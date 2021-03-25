/**
 * @author      : rbrugo, momokrono
 * @file        : line
 * @created     : Friday Mar 05, 2021 12:31:51 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_LINE_HPP
#define PRIMITIVES_LINE_HPP

// #include <ranges>

#include <algorithm>

#include "spl/rgba.hpp"
#include "spl/primitives/vertex.hpp"
#include "spl/image.hpp"

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

struct line
{
    vertex start;
    vertex end;
    rgba color;
    bool anti_aliasing;
    int16_t thickness = 1;

    constexpr line(
        vertex from, vertex to, int16_t thickness,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : start{from}, end{to}, color{color}, anti_aliasing{anti_aliasing}, thickness{thickness}
    {}

    constexpr line(
        vertex from, vertex to,
        spl::graphics::rgba color = spl::graphics::color::black,
        bool anti_aliasing = true
    ) : line{from, to, 1, color, anti_aliasing}
    {}

    void render_on(image & img) const noexcept;

    void draw_antialiased_parametric(image & img) const noexcept;
    void draw_aliased(image & img) const noexcept;
    void draw_antialiased(image & img) const noexcept;

    constexpr
    auto translate(int_fast32_t x, int_fast32_t y) noexcept -> line & {
        start += {x, y};
        end += {x, y};
        return *this;
    }

private:
    void _draw_thick(image & img) const noexcept;
};

} // namespace spl::graphics

#endif /* PRIMITIVES_LINE_HPP */

