#include "plotter.hpp"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <fstream>

#include <range/v3/view/chunk.hpp>

namespace spl
{
    auto plotter::save_as_ppm(std::filesystem::path const & destination, sf::Image const & img)
        -> bool
    {
        auto sink = std::ofstream{destination};
        if (not sink) {
            return false;
        }

        // PMM FILE FORMAT
        // 1) 'P3'
        // 2) width height
        // 3) max_scale_factor (255)
        // 4) data... 1 line x pixel
        auto const [width, height] = img.getSize();
        fmt::print(sink, "P3\n{} {}\n255\n", width, height);

        for (size_t j = 0; j < height; ++j) {
            for (size_t i = 0; i < width; ++i) {
                auto const [r, g, b, a] = img.getPixel(i, j);
                fmt::print(sink, "{} {} {}\n", r, g, b);
            }
            fmt::print(sink, "\n");
        }

        return true;
    }

    auto plotter::save_plot(std::filesystem::path const & name) const
            -> bool
    {
        auto const & texture = get_texture();
        auto const   pic = texture.copyToImage();

        if (name.extension() == ".ppm") {
            return save_as_ppm(name, pic);
        } else if (pic.saveToFile(name))
        {
            fmt::print("plot saved\n"); // TODO: aggiungere eventualmente un testo su schermo
            return true;
        } else {
            fmt::print("errors occurred while saving the plot\n");
            return false;
        }
    }

} // namespace spl
