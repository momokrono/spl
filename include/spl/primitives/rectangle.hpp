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

/**
  Represents a `rectangle` to be drawed on an `image`
 * */
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
    /// @name Constructors
    /// @{
    /**
      Constructs a rectangle

      @param origin the top-left vertex of the rectangle
      @param width the rectangle width
      @param height the rectangle height
      @param rotation the inclination angle of the rectangle (in radians)
      @param antialiasing enable antialiasing when drawing the rectangle
     * */
    constexpr
    rectangle(vertex const origin, int_fast32_t width, int_fast32_t height, double rotation = 0., bool antialiasing = false) noexcept :
        _origin{origin}, _sides{width, height}, _rotation(rotation), _anti_aliasing{antialiasing}
    {}

    /**
      Constructs a rectangle

      @param top_left the top-left vertex of the rectangle
      @param bot_right the bottom-right vertex of the rectangle, will be used to compute a width and an height
      @param rotation the inclination angle of the rectangle (in radians)
      @param antialiasing enable antialiasing when drawing the rectangle
     * */
    constexpr
    rectangle(
        vertex const top_left, vertex const bot_right, double rotation = 0., bool antialiasing = false
    ) noexcept :
        _origin{top_left},
        _sides{top_left.x - bot_right.x + 1, top_left.y - bot_right.y + 1},
        _rotation(rotation),
        _anti_aliasing{antialiasing}
    {}
    /// @}

    /// @name Coloring
    /// @{
    /**
      Set the color that will be used to draw the border

      @param fill the color
      @returns `*this`
     */
    constexpr
    auto border_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _border_color = fill; return *this; }

    /**
      Set the color that will be used to fill the rectangle

      @param fill the color
      @returns `*this`
     */
    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> rectangle &
    { _fill_color = fill; return *this; }
    /// @}

    /// @name Drawing
    /// @{
    /// Draw the rectangle on a given viewport
    void render_on(viewport img) const noexcept;
    /// @}

    /**
      Translate the rectangle

      @param x how much the rectangle must be translated along the x axis
      @param y how much the rectangle must be translated along the y axis
      @returns `*this`
     * */
    constexpr
    auto translate(int_fast32_t x, int_fast32_t y) noexcept -> rectangle & {
        _origin += vertex{x, y};
        return *this;
    }

    /**
      Set a new origin for the rectangle

      @param x the new position of the origin along the x axis
      @param y the new position of the origin along the y axis
      @returns `*this`
     * */
    constexpr
    auto set_origin(int_fast32_t const x, int_fast32_t const y) noexcept -> rectangle & {
        _origin = vertex{x, y};
        return *this;
    }

    /**
      Set a new origin for the rectangle

      @param new_orig the new position of the origin
      @returns `*this`
     * */
    constexpr
    auto set_origin(vertex const new_orig) noexcept -> rectangle & {
        _origin = new_orig;
        return *this;
    }

    /**
      Increase the rotation angle for the rectangle

      @param angle the amount of radians to be added to the current rotation
      @returns `*this`
     * */
    constexpr
    auto rotate(float const angle) noexcept -> rectangle & {
        _rotation += angle;
        return *this;
    }

    /**
      Set the rotation angle for the rectangle

      @param angle the new rotation of the rectangle
      @returns `*this`
     * */
    constexpr
    auto set_rotation(float const angle) noexcept -> rectangle & {
        _rotation = angle;
        return *this;
    }

};

} // namespace spl::graphics

#endif /* PRIMITIVES_RECTANGLE_HPP */
