/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * @file        : bench
 * @created     : Friday Jun 12, 2020 14:13:07 CEST
 * @license     : MIT
 */

#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>

#include "spl/image.hpp"
#include "spl/primitive.hpp"

int main(int argc, [[maybe_unused]] char * argv[]) try
{
    auto image = spl::graphics::image(600,400);

    fmt::print(stdout, argc == 1 ? "new\n" : "old\n");
    for (int j = 0; j <= 66; ++j) {
        auto line1 = spl::graphics::line{{0,   200}, {599, 200 + j * 3 }, spl::graphics::color::green};
        auto line2 = spl::graphics::line{{599, 200}, {  0, 200 + j * 3 }, spl::graphics::color::blue};
        auto line3 = spl::graphics::line{{0,   200}, {599, 200 - j * 3 }, spl::graphics::color::green};
        auto line4 = spl::graphics::line{{599, 200}, {  0, 200 - j * 3 }, spl::graphics::color::blue};
        if (argc == 1) {
            line1.draw_antialiased_parametric(image);
            line2.draw_antialiased_parametric(image);
            line3.draw_antialiased_parametric(image);
            line4.draw_antialiased_parametric(image);
        } else {
            line1.draw_antialiased(image);
            line2.draw_antialiased(image);
            line3.draw_antialiased(image);
            line4.draw_antialiased(image);
        }
    }

    if (not image.save_to_file("a.png")) { fmt::print("NON VA\n"); };
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}


