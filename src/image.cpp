/**
 * @author      : momokrono, rbrugo
 * @file        : image.cpp
 * @created     :
 * @license     : MIT
 */

#include "spl/image.hpp"
#include "spl/viewport.hpp"
#include "spl/detail/exceptions.hpp"

#include <fstream>
#include <fmt/core.h>
#include <fmt/os.h>

#ifdef SPL_FILL_MULTITHREAD
#include <thread>
#endif // SPL_FILL_MULTITHREAD

#include "stb_image_write.h"
#include "stb_image.h"

namespace spl::graphics
{

auto image::get_pixel_iterator(index_type const x, index_type const y)
    -> image::iterator
{
    if (x > _width - 1 or y > _height - 1) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.begin() + static_cast<ptrdiff_t>(x + y * _width);
}

auto image::get_pixel_iterator(index_type const x, index_type const y) const
    -> image::const_iterator
{
    if (x > _width - 1 or y > _height - 1) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.begin() + static_cast<ptrdiff_t>(x + y * _width);
}

auto image::rows() -> spl::graphics::image_range<true, false>
{
    return {row(0), height()};
}

auto image::rows() const -> spl::graphics::image_range<true, true>
{
    return {row(0), height()};
}

auto image::columns() -> spl::graphics::image_range<false, false>
{
    return {column(0), width()};
}

auto image::columns() const -> spl::graphics::image_range<false, true>
{
    return {column(0), width()};
}

auto image::row(size_t const y) -> spl::graphics::row_col_range<true, false>
{
    auto it = get_pixel_iterator(0, y);
    return {it, _width};
}

auto image::row(size_t const y) const -> spl::graphics::row_col_range<true, true>
{
    auto it = get_pixel_iterator(0, y);
    return {it, _width};
}

auto image::column(size_t const x) -> spl::graphics::row_col_range<false, false>
{
    auto it = get_pixel_iterator(x, 0);
    return {{it, _width}, _height};
}

auto image::column(size_t const x) const -> spl::graphics::row_col_range<false, true>
{
    auto it = get_pixel_iterator(x, 0);
    return {{it, _width}, _height};
}

auto image::pixel(index_type const x, index_type const y) const
    -> const_reference
{
    if (x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x+y*_width);
}

auto image::pixel(index_type const x, index_type const y)
    -> reference
{
    if (x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x + y * _width);
}

auto image::pixel_noexcept(index_type const x, index_type const y) const noexcept
    -> const_reference
{
    if (x >= width() or y >= height()) {
        return image::_garbage_pixel;
    }
    return _pixels.at(x + y * _width);
}

auto image::pixel_noexcept(index_type const x, index_type const y) noexcept
    -> reference
{
    if (x >= width() or y >= height()) {
        return image::_garbage_pixel;
    }
    return _pixels.at(x + y * _width);
}

auto image::fill(rgba const c) & noexcept
    -> image &
{
#ifdef SPL_FILL_MULTITHREAD
    auto const n_threads = std::thread::hardware_concurrency();
    auto const pix_dim = _height * _width;
    auto const pix_per_thread = pix_dim / n_threads;
    auto jobs = std::vector<std::jthread>();
    jobs.reserve(n_threads);
    for (auto i = 0u; i < n_threads; ++i) {
        jobs.emplace_back([this](auto j, auto p, auto col) {
            std::ranges::fill(_pixels.begin() + p * j, _pixels.begin() + p * (j + 1), col);
        }, i, pix_per_thread, c);
    }
    std::ranges::fill(_pixels.begin() + pix_per_thread * n_threads, _pixels.end(), c);
#else
    std::ranges::fill(_pixels, c);
#endif // SPL_FILL_MULTITHREAD

    return *this;
}

bool image::save_to_file(std::string_view const filename) const
{
    if (filename.ends_with(".bmp")) {
        return stbi_write_bmp(filename.data(), swidth(), sheight(), 4, raw_data()) == 1;
    }
    if (filename.ends_with(".png")) {
        return stbi_write_png(filename.data(), swidth(), sheight(), 4, raw_data(), 0) == 1;
    }
    if (filename.ends_with(".jpg")) {
        return stbi_write_jpg(filename.data(), swidth(), sheight(), 4, raw_data(), 90) == 1;
    }
    if (filename.ends_with(".ppm")) {
        return save_to_ppm(filename);
    }
    if (filename.ends_with(".qoi")) {
        return save_to_qoi(filename);
    }
#if 0
    if (filename.ends_with(".pam")) {
        auto sink = std::ofstream{filename.data()};
        if (not sink) {
            return false;
        }

        fmt::print(sink, "P7\nWIDTH {}\nHEIGHT {}\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n", width(), height());

        for (size_t j = 0; j < height(); ++j) {
            for (size_t i = 0; i < width(); ++i) {
                auto const [r, g, b, a] = pixel(i, j);
                fmt::print(sink, "{} {} {} {}\n", r, g, b, a);
            }
        }

        return true;
    }
#endif
    return false;
}

bool image::save_to_ppm(std::string_view const filename) const
{
    auto sink = fmt::output_file(filename.data());
    sink.print("P3\n{} {}\n255\n", width(), height());
    for (size_t j = 0; j < height(); ++j) {
        for (size_t i = 0; i < width(); ++i) {
            auto const [r, g, b, a] = pixel(i, j);
            sink.print("{} {} {}\n", a * r / 255, a * g / 255, a * b / 255);
        }
        sink.print("\n");
    }

    return true;
}

bool image::save_to_qoi(std::string_view const filename) const
{
    auto sink = std::ofstream(filename.data(), std::ios::out | std::ios::binary);

    constexpr auto QOI_OP_INDEX = 0x00;
    constexpr auto QOI_OP_DIFF  = 0x40;
    constexpr auto QOI_OP_LUMA  = 0x80;
    constexpr auto QOI_OP_RUN   = 0xc0;
    constexpr auto QOI_OP_RGBA  = 0xff;

    constexpr auto pixel_hash = [](spl::graphics::rgba const p){
        return (p.r * 3 + p.g * 5 + p.b * 7 + p.a * 11) % 64;
    };

    auto encoded_data = std::vector<uint8_t>{};
    encoded_data.reserve(14 + _width * _height * 5 + 8);

    encoded_data.push_back('q');
    encoded_data.push_back('o');
    encoded_data.push_back('i');
    encoded_data.push_back('f');

    encoded_data.push_back(( _width & 0xFF000000) >> 24);
    encoded_data.push_back(( _width & 0x00FF0000) >> 16);
    encoded_data.push_back(( _width & 0x0000FF00) >>  8);
    encoded_data.push_back(( _width & 0x000000FF) >>  0);
    encoded_data.push_back((_height & 0xFF000000) >> 24);
    encoded_data.push_back((_height & 0x00FF0000) >> 16);
    encoded_data.push_back((_height & 0x0000FF00) >>  8);
    encoded_data.push_back((_height & 0x000000FF) >>  0);

    encoded_data.push_back(4);
    encoded_data.push_back(1);

    auto indexes = std::array<spl::graphics::rgba, 64>{{{0, 0, 0, 0}}};
    auto prev_pixel = spl::graphics::rgba(0, 0, 0, 255);
    uint8_t run = 0;
    for (auto & p : _pixels) {
        if (p == prev_pixel) {
            run++;
            if (run == 62 || &p == &_pixels.back()) {
                encoded_data.push_back(QOI_OP_RUN | (run - 1));;
                run = 0;
            }
            continue;
        }
        if (run > 0) {
            encoded_data.push_back(QOI_OP_RUN | (run - 1));;
            run = 0;
        }
        auto idx = pixel_hash(p);
        if (p == indexes[idx]) {
            encoded_data.push_back(QOI_OP_INDEX | idx);
            prev_pixel = p;
            continue;
        }
        indexes[idx] = p;
        if (p.a == prev_pixel.a) {
            auto dr = p.r - prev_pixel.r;
            auto dg = p.g - prev_pixel.g;
            auto db = p.b - prev_pixel.b;
            auto dr_g = dr - dg;
            auto db_g = db - dg;
            if (
                dr > -3 && dr < 2 &&
                db > -3 && db < 2 &&
                dg > -3 && dg < 2
               ) {
                encoded_data.push_back(QOI_OP_DIFF | (dr + 2) << 4 | (dg + 2) << 2 | (db + 2));
            }
            else if (
                     dr_g >  -9 && dr_g < 8  &&
                     dg   > -33 && dg   < 32 &&
                     db_g >  -9 && db_g < 8
                    ) {
                encoded_data.push_back(QOI_OP_LUMA | (dg + 32));
                encoded_data.push_back((dr_g + 8) << 4 | (db_g + 8));
            }
            else {
                encoded_data.push_back(QOI_OP_RGBA);
                encoded_data.push_back(p.r);
                encoded_data.push_back(p.g);
                encoded_data.push_back(p.b);
                encoded_data.push_back(p.a);
            }
        }
        prev_pixel = p;
    }

    auto padding = std::array<uint8_t, 8>{0,0,0,0,0,0,0,1};
    for (auto & p : padding) {
        encoded_data.push_back(p);
    }

    if ( not sink.write(reinterpret_cast<const char*>(encoded_data.data()),
                        static_cast<std::streamsize>(encoded_data.size()))) {
        fmt::print("error saving file \n");
    }

    return true;
}

auto image::load_from_file(std::filesystem::path const & filename)
    -> load_status
{
    struct stb_clear { void operator()(uint8_t * ptr) { if (ptr) { stbi_image_free(ptr); } } };
    _pixels.clear();
    _width  = 0;
    _height = 0;
    if (not std::filesystem::exists(filename)) {
        return load_status::file_not_found;
    }
    if (filename.extension() == ".ppm") {
        return load_ppm(filename);
    }
    auto width = 0;
    auto height = 0;
    auto channels = 0;
    auto ptr = std::unique_ptr<uint8_t, stb_clear>{
        stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha)
    };

    if (not ptr) {
        return load_status::failure;
    }

    _pixels.resize(static_cast<size_t>(width * height));
    std::ranges::copy_n(ptr.get(), width * height * 4, reinterpret_cast<uint8_t *>(_pixels.data()));
    _width  = static_cast<size_t>(width);
    _height = static_cast<size_t>(height);

    return load_status::success;
}

auto image::load_ppm(std::filesystem::path const & filename)
    -> load_status
{
    auto in = std::ifstream{filename};
    if (not in) {
        return load_status::failure;
    }
    char check[3] = "\0\0";
    in.get(check[0]).get(check[1]).get(check[2]);
    if (check[0] != 'P' or check[1] != '3' or check[2] != '\n') {
        return load_status::failure;
    }
    in.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    auto width  = 0ul;
    auto height = 0ul;
    auto max_color = 0;
    in >> width >> height >> max_color;
    auto buffer = std::vector<rgba>{};
    buffer.reserve(width * height);
    for (size_t i = 0; i < width * height; ++i) {
        uint16_t r, g, b;
        in >> r >> g >> b;
        buffer.emplace_back(r, g, b, 255);
    }

    _pixels = std::move(buffer);
    _width = width;
    _height = height;
    return load_status::success;
}

image::operator basic_viewport<false>() & noexcept
{
    return basic_viewport<false>{*this};
}

image::operator basic_viewport<true>() const & noexcept
{
    return basic_viewport<true>{*this};
}

} // namespace spl::graphics
