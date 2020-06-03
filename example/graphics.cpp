#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>

#include "graphics/Image.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() try
{
    auto image = spl::graphics::image(300,200);
    auto col_1 = image.column(0);
    auto row_1 = image.row(0);
    image.fill({0, 255, 255, 255});
    std::ranges::fill(row_1, spl::graphics::rgba{255, 0, 0, 255});
    std::ranges::fill(col_1, spl::graphics::rgba{255, 0, 0, 255});

    image.row(3)[5] = spl::graphics::rgba{255, 0, 0, 0};
    image.column(5).at(5) = spl::graphics::rgba{255, 0, 0, 0};

    for (size_t i = 0; i < image.height(); ++i) {
        for (size_t j = 0; j < image.width(); ++j) {
            if (i < 20 and j > 180) {
                image.pixel(j, i) = spl::graphics::rgba{255, 255, 0};
            }
        }
    }

    if (not image.save_to_file("a.bmp")) { fmt::print("NON VA\n"); };
    // image.save_to_file("a.jpg");
    // image.save_to_file("a.png");
    // image.save_to_file("a.ppm");
    // image.save_to_file("a.pam");

    // image.pixel(88, 99);
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}
