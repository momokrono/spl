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

class bezier
{
    vertex _anchor_1, _control_1, _control_2, _anchor_2;
    int16_t _thickness;
    int _num_of_points;
    rgba _color;
    bool _anti_aliasing = true;

public:
//    template <std::ranges::range Rng = std::initializer_list<vertex>>
//        requires std::same_as<std::ranges::range_value_t<Rng>, vertex>
//    constexpr
//    bezier(Rng && rng, rgba color = graphics::color::black, bool anti_aliasing = true) noexcept :
//        _vertexes(rng.begin(), rng.end()), _color{color}, _anti_aliasing{anti_aliasing} {}


    bezier(vertex const p0, vertex const p1, vertex const p2, vertex const p3,
           int16_t thickness = 1, int points = 40, rgba color = graphics::color::black, bool anti_aliasing = true) noexcept :
        _anchor_1{p0}, _control_1{p1}, _control_2{p2}, _anchor_2{p3},
        _thickness{thickness}, _num_of_points{points}, _color{color}, _anti_aliasing{anti_aliasing} {}

    constexpr
    auto color(spl::graphics::rgba const color) noexcept
        -> bezier &
    { _color = color; return *this; }


    void render_on(viewport img) const noexcept
    {
        auto t = 0.;
        auto increment = 1. / static_cast<double>(_num_of_points);
        auto start_point = _anchor_1;
        auto end_point = _anchor_1;
        while (t <= 1.0001) {
            auto step = 1. - t;
            auto step2 = step * step;
            auto step3 = step2 * step;
            auto a = step3;
            auto b = 3 * step2 * t;
            auto c = 3 * step * t * t;
            auto d = t * t * t;
            auto end_point_x = a * _anchor_1.x + b * _control_1.x + c * _control_2.x + d * _anchor_2.x;
            auto end_point_y = a * _anchor_1.y + b * _control_1.y + c * _control_2.y + d * _anchor_2.y;
            end_point = {static_cast<long>(std::round(end_point_x)), static_cast<long>(std::round(end_point_y))};
            img.draw(spl::graphics::line(start_point, end_point, _thickness, _color, _anti_aliasing));
//            img.draw(spl::graphics::circle(start_point, 2).fill_color(spl::graphics::color::red));
            start_point = end_point;
            t += increment;
        }
//        img.draw(spl::graphics::circle(start_point, 2).fill_color(spl::graphics::color::red));
    }
};



} // namespace spl::graphics

#endif /* BEZIER_HPP */

