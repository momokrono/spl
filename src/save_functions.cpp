#include "plotter.hpp"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <fstream>

#include <range/v3/view/chunk.hpp>

namespace spl
{
    auto plotter::save_as_pmm(std::filesystem::path const & destination) const
        -> bool
    {
#warning remake this
        return true;
        /*
        namespace rvw = ranges::views;
        auto sink = std::ofstream{destination};
        if (not sink) {
            return false;
        }

        // assume: _xs.begin() != _xs.end();
        auto buffer = std::vector<uint8_t>(_width * _height * 3, 0xFF);
        auto vectors = get_plot_points();

        for (auto const [x, y] : vectors) {
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
        fmt::print(sink, "P3\n{} {}\n255\n", _width, _height);
        for (auto const pixel : buffer | rvw::chunk(3)) {
            fmt::print(sink, "{} {} {}\n", pixel[0], pixel[1], pixel[2]);
        }

        return true;*/
    }

    auto plotter::save_plot( std::string const & name ) const
            -> bool
    {
        auto const texture = get_texture();
        auto pic = texture.copyToImage();
        if (pic.saveToFile(name))
        {
            fmt::print("plot saved\n"); // TODO: aggiungere eventualmente un testo su schermo
            return true;
        } else {
            fmt::print("errors occurred while saving the plot\n");
            return false;
        }
    }

} // namespace spl
