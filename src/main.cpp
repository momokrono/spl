/**
 * @author      : rbrugo, momokrono
 * @file        : main
 * @created     : Monday Mar 30, 2020 21:51:54 CEST
 */

#include <cmath>

#include <range/v3/view/linear_distribute.hpp>
#include <range/v3/view/transform.hpp>

#include "plotter.hpp"

int main()
{
    namespace rvw = ranges::views;
    auto xs = rvw::linear_distribute(-2*3.1415, 2*3.1415, 1000) | ranges::to_vector; //std::vector<double>{};
    auto ys = xs | rvw::transform([](auto x) { return std::cos(x); }) | ranges::to_vector;

    auto plot = spl::plotter{640, 480, xs, ys /* altri dati */};

    // --- file
//     if (plot.save_as_pmm("ok.pmm")) {
//         return 0;
//     }

//     return 1;

    plot.show();
}