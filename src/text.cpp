/**
 * @author      : rbrugo, momokrono
 * @file        : text
 * @created     : Wednesday Mar 16, 2022 10:37:03 CET
 * @description : implements the functionalities of the text class, used to render text on an image
 */

#include <fstream>

#include "stb_truetype.h"
#include "spl/text.hpp"

namespace spl::graphics
{

namespace detail
{
class font_face_impl
{
    friend class spl::graphics::font_face;
    friend std::shared_ptr<font_face_impl> load_from_file(std::filesystem::path const &);

    stbtt_fontinfo _info;
    std::vector<unsigned char> _buffer;

    font_face_impl() = default;

};

auto load_from_file(std::filesystem::path const & source)
    -> std::shared_ptr<font_face_impl>
{
    if (not std::filesystem::exists(source)) {
        throw font_error(fmt::format("font \"{}\" not found", source.native()));
    }
    auto res = std::shared_ptr<font_face_impl>(new font_face_impl);
    {
        auto in = std::ifstream{source, std::ios::in | std::ios::binary | std::ios::ate};
        auto const font_size = in.tellg();
        in.seekg(0, std::ios::beg);
        res->_buffer.resize(font_size);
        in.read(reinterpret_cast<char *>(res->_buffer.data()), font_size);
    }
    auto ec = stbtt_InitFont(&res->_info, res->_buffer.data(), 0);
    if (ec == 0) {
        throw font_error(fmt::format("unexpected error while reading the font \"{}\"", source.native()));
    }
    return res;
}

} // namespace detail

auto font_face::face_info() const -> decltype(auto) { return std::addressof(_impl->_info); }
auto font::face_info()      const -> decltype(auto) { return _face.face_info(); }

namespace detail
{
struct codepoint_result
{
    int32_t codepoint;
    uint8_t size;
};

constexpr
auto parse_codepoint(char const * it, char const * end)
    -> codepoint_result
{
    auto ch = static_cast<uint8_t>(*it);
    if (ch >> 7 == 0) {
        return {static_cast<int32_t>(ch), 1};
    }
    auto const distance = end - it;
    auto valid_cp = [](auto it_) { return (*it_ >> 6) != 0b10; };
    auto mask = [](auto it_) { return *it_ & 0b00111111; };

    if ((ch >> 5) == 0b110) {
        if (distance < 2 or not valid_cp(it + 1)) {
            throw utf8_error("truncated UTF-8 encoded character");
        }
        return {(ch & 0b00011111) << 6 | mask(it + 1), 2};
    }
    if ((ch >> 4) == 0b1110) {
        if (distance < 3 or not valid_cp(it + 1) or not valid_cp(it + 2)) {
            throw utf8_error("truncated UTF-8 encoded character");
        }
        return {(ch & 0b00001111) << 12 | (mask(it + 1) << 6) | mask(it + 2) , 3};
    }
    if ((ch >> 3) == 0b11110) {
        if (distance < 4 or not valid_cp(it + 1) or not valid_cp(it + 2) or not valid_cp(it + 3)) {
            throw utf8_error("truncated UTF-8 encoded character");
        }
        return {(ch & 0b111) << 18 | (mask(it + 1) << 12) | (mask(it + 2) << 6) | mask(it + 3), 4};
    }
    throw utf8_error("invalid UTF-8 character");
}

auto parse_codepoints(std::string_view text)
{
    auto res = std::vector<int32_t>{};
    auto it = text.begin();
    auto const end = text.end();
    while (it != end) {
        auto [codepoint, cp_size] = parse_codepoint(it, end);
        res.push_back(codepoint);
        std::ranges::advance(it, cp_size, end);
    }
    return res;
}

struct stb_deleter
{
    template <typename T>
    constexpr auto operator()(T * ptr) const noexcept {
        free(ptr);
    }
};
} // namespace detail

void text::render_on(viewport img) const noexcept
{

    auto [x_pos, advance, lsb, x0, y0, x1, y1] = std::array{0, 0, 0, 0, 0, 0, 0};
    auto const codepoints = detail::parse_codepoints(_text);
    auto const end = codepoints.end();
    auto const color = _color;
    for (auto cp_it = codepoints.begin(); cp_it != end; ++cp_it) {
        auto codepoint = static_cast<int32_t>(*cp_it);
        auto const x_shift = x_pos - std::floor(x_pos);
        stbtt_GetCodepointHMetrics(_font.face_info(), codepoint, &advance, &lsb);
#if defined SPL_TODO_SHOULD_USE_THIS_TO_AVOID_LOTS_OF_REALLOCATIONS
        stbtt_GetCodepointBitmapBoxSubpixel(
            _font._face_info(), *ch, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1
        );
        // TODO: pre-allocate an output buffer in the font class, eventually
        stbtt_MakeCodepointBitmapSubpixel/*Prefilter*/(
            _font._face_info(),  // info
            &out,                // output
            x1 - x0, y1 - y0,    // out width, height
            99,                  // out stride (?)
            scale, scale,        // scale x, y
            x_shift, 0,          // shift x, y
            codepoint            // codepoint
        );
#else
        auto scale = stbtt_ScaleForPixelHeight(_font.face_info(), _font._height);
        auto [width, height, x_off, y_off] = std::array{0, 0, 0, 0};
        auto data = std::unique_ptr<unsigned char [], detail::stb_deleter>(stbtt_GetCodepointBitmapSubpixel(
            _font.face_info(),  // info
            scale, scale,       // scale x, y
            x_shift, 0,         // shift x, y
            codepoint,          // codepoint (int)
            &width, &height,    // width, height (out params)
            &x_off, &y_off      // offset from the top-left corner (out params)
        ));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto & pixel = img.pixel(
                    _origin.x + x_pos + x0 + x + x_off,
                    _origin.y + y0 + y + y_off
                );
                pixel = spl::graphics::over(color.blend(data[x + width * y]), pixel);
            }
        }
#endif
        x_pos += advance * scale;
        if (cp_it + 1 != end) {
            auto next_codepoint = static_cast<uint32_t>(*(cp_it + 1));
            x_pos += scale * stbtt_GetCodepointKernAdvance(_font.face_info(), codepoint, next_codepoint);
        }
    }
}

} // namespace spl::graphics
