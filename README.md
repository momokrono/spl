[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)

# `spl` - A Simple Plotting Library for C++

**The library is under heavy development, some parts are broken, others may not work from time to time.
If you find yourself having issues with it, feel free to report it.**


## About

`spl` aims to be a simple library to draw stuff and edit images in C++. It is developed with the following ideas in mind:

- Ease of use: the library must be easy to install and easy to use. We're achieving the former using the [conan package manager](https://conan.io/) for the dependencies and [cmake](https://cmake.org/) as build system, and the latter by taking inspiration from [matplotlib](https://matplotlib.org/), since we recognize in it the simplicity wewant our library to transmit while using it
- Being as up-to-date with the standard as possible: C++ is evolving, and there's no reason for us to avoid all the goodies that C++11, 14, 17 and 20 have brought us. Also, that's a good exercise to learn all the new stuff that comes out from every new standard
- Being as standard as possible (minimal external dependencies): we want `spl` to be as standalone as possible. As for now, we have the following dependencies:
    - [range-v3](https://github.com/ericniebler/range-v3), waiting for the C++20 ranges library
    - [fmtlib](https://github.com/fmtlib/fmt) in the absence of the C++20 formatting library
    - [stb](https://github.com/nothings/stb) for saving and loading various image types

## Example

```cpp
#include <ranges>
#include <numbers>
#include <iostream>

#include "spl/graphics/image.hpp"
#include "spl/graphics/primitive.hpp"

namespace color = spl::graphics::color;

int main()
{
    // Creates a new image
    auto image = spl::graphics::image(900, 600);

    // Support for rows, columns, pixels, and for row-by-row, column-by-column, pixel-by-pixel iteration
    std::ranges::fill(image.row(449), spl::graphics::rgba{255, 0, 255, 255});
    std::ranges::fill(image.column(130), color::violet);

    // Supports some drawing primitives, like lines and rectangles
    auto line = spl::graphics::line{{0, 0}, {300, 599}, color::cyan};
    image.draw(line);
    image.draw(spl::graphics::line{{100, 100}, {50, 550}, color::orange});

    auto rect = spl::graphics::rectangle{{150, 150}, {90, 30}};
    rect.fill_color(color::green);
        auto rotated_rect = spl::graphics::rectangle{{20, 30}, {100, 40}, std::numbers::pi / 4, /*antialiasing=*/true}
                      .fill_color(color::red);
    image.draw(rect)
         .draw(rotated_rect);

	// Save a new image
    if (not image.save_to_file("image.png")) {
        std::cerr << "Error\n";
        return 1;
    }

    // Load an existing image
    auto status = image.load_from_file("image.png");
    if (status != spl::graphics::load_status::success) {
        std::cerr << "Error while loading a.png";
        return 2;
    }
}

```