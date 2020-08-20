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
    inline
    auto top_vertex(std::initializer_list<vertex> const & lst) noexcept
        -> vertex
    { return std::ranges::max(lst, std::less{}, &vertex::y); }

    inline
    auto segment_compare = [](std::pair<vertex, vertex> const & a, std::pair<vertex, vertex> const & b)
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

class circle : public primitive
{
    bool filled = false;
    rgba color;
    uint_fast32_t radius;
    vertex center;
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
    rectangle(vertex const origin, uint_pair const sides, double rotation = 0., bool antialiasing = false) :
        _origin{origin}, _sides{sides}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    auto border_color(spl::graphics::rgba const fill)
        -> rectangle &
    { _border_color = fill; return *this; }

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
    regular_polygon(vertex const origin, int_fast32_t const radius, uint8_t const sides, double rotation = 0., bool antialiasing = false) :
        _center{origin}, _sides{sides}, _radius{radius}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    auto border_color(spl::graphics::rgba const fill)
        -> regular_polygon &
    { _border_color = fill; return *this; }

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
            /* _draw_filled(img); */
        } else {
            _draw_unfilled(img);
        }
    }

private:
    void _draw_unfilled(image & img);
};

} // namespace spl::graphics

#endif /* PRIMITIVE_HPP */

