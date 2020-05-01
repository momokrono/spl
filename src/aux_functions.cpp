#include "plotter.hpp"

#include <functional>

#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

namespace spl
{
    /*
    template <typename Container1, typename Container2>
        requires ranges::input_iterator<Container1> and ranges::input_iterator<Container2>
    auto plotter::get_plot_points() const
            -> std::vector<std::pair<int, int>>
    {
        namespace rvw = ranges::views;
        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_max] = std::minmax_element(_ys.begin(), _ys.end());
        auto const xs = _xs | rvw::transform(std::bind_front(&plotter::rescale, this, *x_min, *x_max, _width));
        auto const ys = _ys | rvw::transform(std::bind_front(&plotter::rescale, this, *y_min, *y_max, _height));

        return rvw::zip(xs, ys) | ranges::to<std::vector<std::pair<int, int>>>;
    }*/
}
