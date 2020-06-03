#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>
#include "Graphics/Image.hpp"

int main() try
{
    auto image = spl::graphics::image(30,20);
    auto col_1 = image.column(0);
    auto row_1 = image.row(0);
    std::ranges::fill(row_1, spl::graphics::rgba{1, 0, 0, 0});
    std::ranges::fill(col_1, spl::graphics::rgba{1, 0, 0, 0});


    image.row(3)[5] = spl::graphics::rgba{1, 0, 0, 0};
    image.column(5).at(5) = spl::graphics::rgba{1, 0, 0, 0};

    for (size_t i = 0; i < image.height(); ++i) {
        for (size_t j = 0; j < image.width(); ++j) {
            fmt::print("{}", image.pixel(j, i) == spl::graphics::rgba{0, 0, 0, 0} ? '.' : 'O');
        }
        fmt::print("\n");
    }

    fmt::print("\n");

    for (int i = 0; i < 20; ++i) {
        for (auto const pixel : image.row(i)) {
            fmt::print("{}", pixel == spl::graphics::rgba{0, 0, 0, 0} ? '.' : 'O');
        }
        fmt::print("\n");
    }

    fmt::print("\n");

    for (auto row : image.rows()) {
        for (auto const pixel : row) {
            fmt::print("{}", pixel == spl::graphics::rgba{0, 0, 0, 0} ? '.' : 'O');
        }
        fmt::print("\n");
    }

    // image.pixel(88, 99);
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}
