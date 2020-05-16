/**
 * @author      : rbrugo, momokrono
 * @file        : main
 * @created     : Monday Mar 30, 2020 21:51:54 CEST
 */

#include <cmath>

#include <range/v3/view/linear_distribute.hpp>
#include <range/v3/view/transform.hpp>

#include "plotter.hpp"

#ifndef __cpp_lib_math_constants
constexpr auto pi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620\
899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110\
555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432\
66482l;
#else
#include <numbers>
constexpr auto pi = std::numbers::pi_v<long double>;
#endif

int main()
{
    namespace rvw = ranges::views;
    auto xs = rvw::linear_distribute(-2*pi, 2*pi, 400) | ranges::to_vector;
    auto ys = xs | rvw::transform([](auto x) { return std::exp(x) / 1e2; }) | ranges::to_vector;
    auto x2 = rvw::linear_distribute(-4*pi, 4*pi, 100) | ranges::to_vector;
    auto y2 = x2 | rvw::transform([](auto x) { return std::sin(x); }) | ranges::to_vector;


    auto plot = spl::plotter{640, 480};
    plot.plot(xs, ys)
        .plot(x2, y2, "ro-")
        ;

    plot.show();
    // plot.save_plot("plot.png");
}
