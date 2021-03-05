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
#include "spl/image.hpp"

namespace spl::graphics
{

class rectangle
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
    auto border_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _border_color = fill; return *this; }

    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _fill_color = fill; return *this; }

    void render_on(image & img) const noexcept;

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

