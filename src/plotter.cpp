/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:08:45 CEST
 * @license     : MIT
 */

#include "plotter.hpp"

#include <fstream>

#include <fmt/format.h>

#include <range/v3/view/transform.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/zip.hpp>

namespace spl
{
    auto plotter::save_as_pmm(std::filesystem::path const & destination) const
        -> bool
    {
        namespace rvw = ranges::views;
        auto sink = std::ofstream{destination};
        // TODO: check if file is good
        // assume: _xs.begin() != _xs.end();
        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_may] = std::minmax_element(_ys.begin(), _ys.end());
        auto const xs = _xs | rvw::transform(std::bind_front(&plotter::rescale, this, *x_min, *x_max, _width));
        auto const ys = _ys | rvw::transform(std::bind_front(&plotter::rescale, this, *y_min, *y_may, _height));

        auto buffer = std::vector<uint8_t>(_width * _height * 3, 0xFF);

        for (auto const [x, y] : rvw::zip(xs, ys)) {
            auto const base_offset = (x + y * _width) * 3;
            buffer.at(base_offset + 0) = 0x00;
            buffer.at(base_offset + 1) = 0x00;
            buffer.at(base_offset + 2) = 0x00;
        }


        // PMM FILE FORMAT
        // 1) 'P3'
        // 2) width height
        // 3) max_scale_factor (255)
        // 4) data... 1 line x pixel
        sink << fmt::format("P3\n{} {}\n255\n", _width, _height);
        for (auto const pixel : buffer | rvw::chunk(3)) {
            sink << fmt::format("{} {} {}\n", pixel[0], pixel[1], pixel[2]);
        }

        return true;
    }
} // namespace spl


