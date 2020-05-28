#include "Graphics/Image.hpp"
#include <iostream>
#include <fmt/format.h>
#include <ranges>
#include <algorithm>

int main()
{
    auto image = spl::graphics::image(20,20);
    auto col_1 = image.access_column(0);
    auto row_1 = image.access_row(0);
    std::ranges::end(col_1);
    std::ranges::transform(col_1,
                            col_1.begin(),
                            [](auto){return spl::graphics::rgba{0,0,0,0};});
}