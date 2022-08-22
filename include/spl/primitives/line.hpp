/**
 * @author      : rbrugo, momokrono
 * @file        : line
 * @created     : Friday Mar 05, 2021 12:31:51 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_LINE_HPP
#define PRIMITIVES_LINE_HPP

#include <algorithm>

#include "spl/rgba.hpp"
#include "spl/primitives/vertex.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

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

/**
  Represents a `line` to be drawed on an `image`.
 * */
struct line
{
    vertex start;
    vertex end;
    rgba color;
    bool anti_aliasing;
    int16_t thickness = 1;

    /// @name Constructors
    /// @{
    /**
      Constructs a `line` (complete configuration)

      @param from the vertex where the line begins
      @param to the vertex where the line ends
      @param thickness the line thickness
      @param color the color that will be applied to the line
      @param anti_aliasing enable the anti-aliasing when drawing the line
     * */
    constexpr line(
        vertex from, vertex to, int16_t thickness,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : start{from}, end{to}, color{color}, anti_aliasing{anti_aliasing}, thickness{thickness}
    {}

    /**
      Constructs a `line`

      Same as the previous, but the thickness is defaulted to 1

      @param from the vertex where the line begins
      @param to the vertex where the line ends
      @param color the color that will be applied to the line
      @param anti_aliasing enable the anti-aliasing when drawing the line
     * */
    constexpr line(
        vertex from, vertex to,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : line{from, to, 1, color, anti_aliasing}
    {}

    /**
      Constructs a `line` (complete configuration, using length and inclination)

      @param from the vertex where the line begins
      @param length the line length
      @param inclination the angle between the line and the x axis, in radians
      @param thickness the line thickness
      @param color the color that will be applied to the line
      @param anti_aliasing enable the anti-aliasing when drawing the line
     * */
    constexpr line(
        vertex from, double length, double inclination,
        int16_t thickness,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : line{
        from,
        vertex(
            std::round(from.x + length * std::sin(inclination)),
            std::round(from.y + length * std::cos(inclination))
        ),
        thickness, color, anti_aliasing
    } {}

    /**
      Constructs a `line`

      Same as the previous, but the thickness is defaulted to 1

      @param from the vertex where the line begins
      @param length the line length
      @param inclination the angle between the line and the x axis, in radians
      @param color the color that will be applied to the line
      @param anti_aliasing enable the anti-aliasing when drawing the line
     * */
    constexpr line(
        vertex from, double length, double inclination,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : line{from, length, inclination, 1, color, anti_aliasing}
    {}
    /// @}

    /// @name Drawing
    /// @{
    /// Draw the line on a given viewport
    void render_on(viewport img) const noexcept;

    void draw_antialiased_parametric(viewport img) const noexcept;
    void draw_aliased(viewport img) const noexcept;
    void draw_antialiased(viewport img) const noexcept;
    /// @}

    /**
      Translate the line

      @param x how much the line must be translated along the x axis
      @param y how much the line must be translated along the y axis
      @returns `*this`
     * */
    constexpr
    auto translate(int_fast32_t x, int_fast32_t y) noexcept -> line & {
        start += {x, y};
        end += {x, y};
        return *this;
    }

private:
    void _draw_thick(viewport img) const noexcept;
};

} // namespace spl::graphics

#endif /* PRIMITIVES_LINE_HPP */

