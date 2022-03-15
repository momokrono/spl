/**
 * @author      : rbrugo, momokrono
 * @file        : rectangle
 * @created     : Friday Mar 05, 2021 12:40:48 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_RECTANGLE_HPP
#define PRIMITIVES_RECTANGLE_HPP

#include <cstdint>
#include <utility>

#include "spl/primitives/vertex.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

class rectangle
{
    vertex _origin;
    std::pair<int_fast32_t, int_fast32_t> _sides;
    float _rotation = 0.;
    rgba _border_color = spl::graphics::color::black;
    rgba _fill_color   = spl::graphics::color::nothing;
    bool _anti_aliasing = false;
    // border_thickness

public:
    constexpr
    rectangle(vertex const origin, int_fast32_t width, int_fast32_t height, double rotation = 0., bool antialiasing = false) noexcept :
        _origin{origin}, _sides{width, height}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    constexpr
    rectangle(
        vertex const upper_left, vertex const lower_right, double rotation = 0., bool antialiasing = false
    ) noexcept :
        _origin{upper_left},
        _sides{upper_left.x - lower_right.x + 1, upper_left.y - lower_right.y + 1},
        _rotation(rotation),
        _anti_aliasing{antialiasing}
    {}

    constexpr
    auto border_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _fill_color = fill; return *this; }

    void render_on(viewport img) const noexcept;

    constexpr
    auto translate(int_fast32_t x, int_fast32_t y) noexcept -> rectangle & {
        _origin += vertex{x, y};
        return *this;
    }

    constexpr
    auto set_origin(int_fast32_t const x, int_fast32_t const y) noexcept -> rectangle & {
        _origin = vertex{x, y};
        return *this;
    }

    constexpr
    auto set_origin(vertex const new_orig) noexcept -> rectangle & {
        _origin = new_orig;
        return *this;
    }

    constexpr
    auto rotate(float const angle) noexcept -> rectangle & {
        _rotation += angle;
        return *this;
    }

    constexpr
    auto set_rotation(float const angle) noexcept -> rectangle & {
        _rotation = angle;
        return *this;
    }

};

} // namespace spl::graphics

#endif /* PRIMITIVES_RECTANGLE_HPP */

