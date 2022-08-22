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

/**
  Represents a `circle` to be drawed on an `image`
 * */
class circle
{
    vertex  _center;
    int_fast32_t _radius;
    rgba _border_color = spl::graphics::color::black;
    rgba _fill_color   = spl::graphics::color::nothing;
    /* bool _anti_aliasing = false; */
public:

    /// @name Constructors
    /// @{
    /**
      Constructs a circle

      @param center the vertex where the center of the circle is located
      @param radius the radius of the circle
     * */
    constexpr
    circle(vertex const center, int_fast32_t radius) noexcept : _center{center}, _radius{radius} {}
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
        -> circle &
    { _border_color = fill; return *this; }

    /**
      Set the color that will be used to fill the circle

      @param fill the color
      @returns `*this`
     */
    constexpr
    auto fill_color(spl::graphics::rgba const fill) noexcept
        -> circle &
    { _fill_color = fill; return *this; }
    /// @}

    /// @name Drawing
    /// @{
    /// Draw the circle on a given viewport
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
    /// @}

    /**
      Translate the circle

      @param x how much the circle must be translated along the x axis
      @param y how much the circle must be translated along the y axis
      @returns `*this`
     * */
    constexpr
    auto traslate(int_fast32_t x, int_fast32_t y) noexcept -> circle & {
        _center += {x, y};
        return *this;
    }

    /**
      Set a new origin for the circle

      @param x the new position of the origin along the x axis
      @param y the new position of the origin along the y axis
      @returns `*this`
     * */
    constexpr
    auto set_origin(int_fast32_t const x, int_fast32_t const y) noexcept -> circle & {
        _center ={x, y};
        return *this;
    }

    /**
      Set a new origin for the circle

      @param new_orig the new position of the origin
      @returns `*this`
     * */
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

