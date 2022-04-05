/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * @file        : bezier
 * @created     : Friday Mar 05, 2021 12:37:18 CET
 * @license     : MIT
 * */

#ifndef BEZIER_HPP
#define BEZIER_HPP

#include <vector>
#include <ranges>
#include <span>

#include "spl/image.hpp"
#include "spl/primitives/vertex.hpp"
#include "spl/primitives/line.hpp"
#include "spl/group.hpp"

namespace spl::graphics
{

namespace detail
{
    void _bezier_render(image & img, std::vector<vertex> const points, spl::graphics::rgba const color, bool const aliased) noexcept;
} // namespace detail

// FIXME
class bezier
{
    std::vector<vertex> _vertexes;
    rgba _color;
    bool _anti_aliasing = true;

public:
    template <std::ranges::range Rng = std::initializer_list<vertex>>
        requires std::same_as<std::ranges::range_value_t<Rng>, vertex>
    constexpr
    bezier(Rng && rng, rgba color = graphics::color::black, bool anti_aliasing = true) noexcept :
        _vertexes(rng.begin(), rng.end()), _color{color}, _anti_aliasing{anti_aliasing} {}


    bezier(std::vector<vertex> & v, rgba color = graphics::color::black, bool anti_aliasing = true) noexcept :
        _vertexes{v}, _color{color}, _anti_aliasing{anti_aliasing} {}

    constexpr
    auto color(spl::graphics::rgba const color) noexcept
        -> bezier &
    { _color = color; return *this; }


    auto render_on(image & img) const noexcept
    {
        if (_vertexes.size() < 2 or _color == spl::graphics::color::nothing) {
            return;
        }
        else {
            detail::_bezier_render(img, _vertexes, _color, _anti_aliasing);
        }
    }
    /*
    constexpr
    auto traslate(int_fast32_t x, int_fast32_t y) noexcept -> line & {
        start += vertex{x, y};
        end += vertex{x, y};
        return *this;
    }
    */
/* private: */
    /* constexpr */
    /* static void _render_on_impl(image & img, std::span<vertex> const) noexcept; */
};



} // namespace spl::graphics

#endif /* BEZIER_HPP */

