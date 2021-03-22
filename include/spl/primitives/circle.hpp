/**
 * @author      : rbrugo, momokrono
 * @file        : circle
 * @created     : Friday Mar 05, 2021 12:54:22 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_CIRCLE_HPP
#define PRIMITIVES_CIRCLE_HPP

#include "spl/primitives/line.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

class circle
{
    vertex  _center;
    int_fast32_t _radius;
    rgba _border_color = spl::graphics::color::black;
    rgba _fill_color   = spl::graphics::color::nothing;
    /* bool _anti_aliasing = false; */
public:

    constexpr
    circle(vertex const center, int_fast32_t radius) noexcept : _center{center}, _radius{radius} {}

    constexpr
    auto border_color(spl::graphics::rgba const fill) noexcept
        -> circle &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> circle &
    { _fill_color = fill; return *this; }

    auto render_on(viewport img) const noexcept
    {
        if (_radius <= 0) {
            return;
        }
        if (_fill_color != spl::graphics::color::nothing) {
            _draw_filled(img);
        } else {
            _draw_unfilled(img);
        }
    }

    constexpr
    auto traslate(int_fast32_t x, int_fast32_t y) noexcept -> circle & {
        _center += {x, y};
        return *this;
    }

    constexpr
    auto set_origin(int_fast32_t const x, int_fast32_t const y) noexcept -> circle & {
        _center ={x, y};
        return *this;
    }

    constexpr
    auto set_origin(vertex const new_orig) noexcept -> circle & {
        _center = new_orig;
        return *this;
    }

private:

    inline
    void _draw_sym_points(viewport img, int_fast32_t const dx, int_fast32_t const dy) const noexcept
    {
        auto const [x0, y0] = _center;
        auto const color    = _border_color;

        auto draw_pixel_with_offset = [&img, x0, y0, color](auto x, auto y) noexcept
        {
            auto & pixel = img.pixel_noexcept(x0 + x, y0 + y);
            pixel = over(color, pixel);
        };

        draw_pixel_with_offset( dx,  dy);
        draw_pixel_with_offset( dx, -dy);
        draw_pixel_with_offset(-dx,  dy);
        draw_pixel_with_offset(-dx, -dy);
    }

    inline
    void _draw_fill_lines(viewport img, int_fast32_t const dx, int_fast32_t const dy) const noexcept
    {
        auto const [x0, y0] = _center;
        auto const color    = _fill_color;

        img.draw(line{{x0 - dx, y0 + dy}, {x0 + dx, y0 + dy}, color});
        img.draw(line{{x0 - dx, y0 - dy}, {x0 + dx, y0 - dy}, color});
    }

    void _draw_unfilled(viewport img) const noexcept;
    void _draw_filled(viewport img) const noexcept;
};

} // namespace spl::graphics

#endif /* PRIMITIVES_CIRCLE_HPP */

