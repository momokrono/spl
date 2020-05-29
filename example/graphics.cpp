#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>
#include "Graphics/Image.hpp"

int main() try
{
    auto image = spl::graphics::image(20,20);
    auto col_1 = image.access_column(0);
    auto row_1 = image.access_row(0);
    std::ranges::fill(row_1, spl::graphics::rgba{1, 0, 0, 0});
    std::ranges::fill(col_1, spl::graphics::rgba{1, 0, 0, 0});

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            fmt::print("{}", image.pixel(j, i) == spl::graphics::rgba{0, 0, 0, 0} ? '.' : 'O');
        }
        fmt::print("\n");
    }

    image.pixel(88, 99);
} catch (std::exception & e)
{
    fmt::print(stderr, "exception: {}\n", e.what());
}
