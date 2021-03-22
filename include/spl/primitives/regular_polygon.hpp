/**
 * @author      : rbrugo, momokrono
 * @file        : regular_polygon
 * @created     : Friday Mar 05, 2021 12:43:29 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_REGULAR_POLYGON_HPP
#define PRIMITIVES_REGULAR_POLYGON_HPP

#include "spl/viewport.hpp"
#include "spl/primitives/vertex.hpp"

namespace spl::graphics
{

struct regular_polygon
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
    auto border_color(spl::graphics::rgba const fill) noexcept
        -> regular_polygon &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> regular_polygon &
    { _fill_color = fill; return *this; }

    inline
    auto render_on(viewport img) const noexcept
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
    void _draw_filled(viewport img) const noexcept;
    void _draw_unfilled(viewport img) const noexcept;
};

} // namespace spl::graphics

#endif /* PRIMITIVES_REGULAR_POLYGON_HPP */

