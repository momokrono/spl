/**
 * @author      : momokrono, rbrugo
 * @file        : primitive
 * @created     : Saturday Jun 06, 2020 23:20:38 CEST
 * @license     : MIT
 * */

#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <cmath>
#include <cstdint>
#include <queue>

#include "rgba.hpp"
#include "image.hpp"

namespace spl::graphics
{

struct vertex
{
    int_fast32_t x;
    int_fast32_t y;
};

namespace detail
{
    constexpr
    auto top_vertex(std::initializer_list<vertex> const & lst) noexcept
        -> vertex
    { return std::ranges::max(lst, std::less{}, &vertex::y); }

    constexpr
    auto segment_compare = [](std::pair<vertex, vertex> const & a, std::pair<vertex, vertex> const & b) noexcept
    {
        auto const [a1, a2] = a;
        auto const [b1, b2] = b;
        if (auto ay = top_vertex({a1, a2}).y, by = top_vertex({b1, b2}).y; ay < by) {
            return true;
        } else if (ay > by) {
            return false;
        } else { // ay == by
            auto const a_bot = std::ranges::min({a1, a2}, std::less{}, &vertex::y).y;
            auto const b_bot = std::ranges::min({b1, b2}, std::less{}, &vertex::y).y;

            return a_bot < b_bot;
        }
    };
} // namespace detail

class primitive
{
public:
};

struct line // : public primitive
{
    vertex start;
    vertex end;
    rgba color;
    bool anti_aliasing = true;
    // uint8_t thickness;

    void render_on(image & img);

    void draw_antialiased_parametric(image & img);
    void draw_aliased(image & img);
    void draw_antialiased(image & img);
};

class rectangle // : public primitive
{
    vertex _origin;
    std::pair<uint_fast32_t, uint_fast32_t> _sides;
    float _rotation = 0.;
    rgba _border_color = spl::graphics::color::black;
    rgba _fill_color   = spl::graphics::color::nothing;
    bool _anti_aliasing = false;
    // border_thickness

public:
    using uint_pair = std::pair<uint_fast32_t, uint_fast32_t>;
    constexpr
    rectangle(vertex const origin, uint_pair const sides, double rotation = 0., bool antialiasing = false) noexcept :
        _origin{origin}, _sides{sides}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    constexpr
    auto border_color(spl::graphics::rgba const fill)
        -> rectangle &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill)
        -> rectangle &
    { _fill_color = fill; return *this; }

    void render_on(image & img);
};

struct regular_polygon // : public primitive
{
private:
    vertex  _center;
    uint8_t _sides;
    int_fast32_t _radius;
    float _rotation = 0.;
    rgba _border_color = spl::graphics::color::black;
    rgba _fill_color   = spl::graphics::color::nothing;
    bool _anti_aliasing;

public:
    constexpr
    regular_polygon(vertex const origin, int_fast32_t const radius, uint8_t const sides, double rotation = 0., bool antialiasing = false) noexcept :
        _center{origin}, _sides{sides}, _radius{radius}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    constexpr
    auto border_color(spl::graphics::rgba const fill)
        -> regular_polygon &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill)
        -> regular_polygon &
    { _fill_color = fill; return *this; }

    inline
    auto render_on(image & img)
    {
        if (_sides == 0 or _radius <= 0) {
            return;
        }
        if (_fill_color != spl::graphics::color::nothing) {
            _draw_filled(img);
        } else {
            _draw_unfilled(img);
        }
    }

private:
    void _draw_filled(image & img);
    void _draw_unfilled(image & img);
};

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
    auto border_color(spl::graphics::rgba const fill)
        -> circle &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill)
        -> circle &
    { _fill_color = fill; return *this; }

    auto render_on(image & img)
    {
        if (_radius <= 0) {
            return;
        }
        if (_fill_color != spl::graphics::color::nothing) {
            /* _draw_filled(img); */
        } else {
            _draw_unfilled(img);
        }
    }

private:

    inline
    void _draw_sym_points(image & img, int_fast32_t dx, int_fast32_t dy)
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
        draw_pixel_with_offset( dy,  dx);
        draw_pixel_with_offset( dy, -dx);
        draw_pixel_with_offset(-dy,  dx);
        draw_pixel_with_offset(-dy, -dx);
    }

    void _draw_unfilled(image & img)
    {
        // Bresenham circle algorithm

        // The circle is divided in 8 symmetric parts
        // The starting point is (x, y)
        // The next point is P':=(x+1, y) or P'':=(x+1, y-1), I have to decide between them
        // We can define a "radius error" function as follow:
        // e(P) = distance(P)² - r² = (x_p)² + (y_p)² - r²
        // With this new function it is possible to define a new "decision parameter":
        //
        // d := e(P') + e(P'')
        //    = (x+1)² + y² - r² + (x+1)² + (y+1)² - r²
        //    = 2x² + 3 + 4x + 2y² - 2y - 2r²
        // If I assume that the first point is (0, r),
        //    = 0   + 3 +  0 + 2r² - 2r - 2r²
        //    = 3 - 2r
        //
        // If d < 0, P' is a better fit than P''. If d >= 0, is the other way.
        //
        // After the first calculation, it can be proved that d must be updated using the following rule:
        //    · d = d + 2*x + 1          if d < 0
        //    · d = d + 2*(x - y) + 1    if d >= 0

        auto const r = _radius; //std::hypot(_center.x, _center.y);
        auto d = 3 - 2 * r;

        auto x = int_fast32_t{0};
        auto y = int_fast32_t(r);
        while (x <= y) {
            _draw_sym_points(img, x, y);
            ++x;
            if (d >= 0) {
                --y;
                d += 2 * (x - y) + 1;
            } else {
                d += 2 * x + 1;
            }
        }
    }
};


} // namespace spl::graphics

#endif /* PRIMITIVE_HPP */

