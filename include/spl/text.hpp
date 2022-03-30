/**
 * @author      : rbrugo, momokrono
 * @file        : text
 * @created     : Friday Mar 25, 2022 14:57:47 CET
 * @description : implements a class to render some text on an image
 * */

#ifndef SPL_TEXT_HPP
#define SPL_TEXT_HPP

#include <map>
#include <string>
#include <memory>
#include <filesystem>

#include "spl/rgba.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

class font;
class font_face;
class text;

namespace detail {
class font_face_impl;
auto load_from_file(std::filesystem::path const & source) -> std::shared_ptr<font_face_impl>;
} // namespace detail

class font_face
{
private:
    friend class font;
    using impl = detail::font_face_impl;

    std::shared_ptr<impl> _impl;

    [[nodiscard]] decltype(auto) face_info() const;
public:
    font_face(font_face &&) = default;
    font_face(font_face const &) = default;
    font_face(std::filesystem::path const & source) : _impl{detail::load_from_file(source)} { }

    auto operator=(font_face &&) -> font_face & = default;
    auto operator=(font_face const &) -> font_face & = default;
};

class font
{
    struct preallocated_codepoint
    {
        preallocated_codepoint(int w, int h, int x, int y, std::shared_ptr<uint8_t []> && d) :
            width(w), height(h), x_off(x), y_off(y), data(std::move(d))
        {}
        int width, height, x_off, y_off;
        std::shared_ptr<uint8_t []> data;
    };

    font_face _face;
    float _height;
    mutable std::map<int32_t, preallocated_codepoint> _buffer;

    friend class text;

    [[nodiscard]] decltype(auto) face_info() const;
public:
    font(font_face face, float height_px) :
        _face(std::move(face)),
        _height{height_px}
    {}

    font(std::filesystem::path const & source, float height_px) :
        font(font_face{source}, height_px)
    {}
};

class text
{
    vertex _origin;
    std::string _text;
    mutable font _font;
    rgba _color;

public:
    text(vertex const & baseline_origin, std::string text, font const & f, rgba color = color::black) :
        _origin{baseline_origin}, _text{std::move(text)}, _font{f}, _color{color}
    {}
    text(
        vertex const & baseline_origin, std::string text, font const & f, float height_px,
        rgba color = color::black
    ) :
        _origin{baseline_origin}, _text{std::move(text)}, _font{f._face, height_px}, _color{color}
    {}
    text(
        vertex const & baseline_origin, std::string text,
        std::filesystem::path const & font_source, float height_px, rgba color = color::black
    ) :
        _origin{baseline_origin}, _text{std::move(text)}, _font{font_source, height_px}, _color{color}
    {}

    void render_on(viewport img) const noexcept;

};

} // namespace spl::graphics

#endif /* SPL_TEXT_HPP */
