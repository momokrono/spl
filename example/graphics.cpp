#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>

#include "spl/image.hpp"
#include "spl/primitive.hpp"

int main() try
{
    auto image = spl::graphics::image(900,600);
    image.fill(spl::graphics::color::white);

    std::ranges::fill(image.row(0), spl::graphics::rgba{255, 0, 255, 255});
    std::ranges::fill(image.column(9), spl::graphics::color::violet);

    image.draw(spl::graphics::line{{0, 0}, {300, 599}, 10, spl::graphics::color::cyan});
    image.draw(spl::graphics::line{{0, 0}, {300, 599}, 3,  spl::graphics::color::green});
    image.draw(spl::graphics::line{{0, 0}, {899, 300}, spl::graphics::color::orange});

    for (float i = 0; i < 3.1415926535 * 4; i += 3.1415296535 / 20) {
        auto const offset = int_fast32_t(i * 50);
        image.draw(spl::graphics::rectangle{{150 + offset, 150 - 15}, 90, 30,  i, true}.fill_color(spl::graphics::color::green));
        image.draw(spl::graphics::rectangle{{150 - offset, 150 - 15}, 90, 30, -i, true}.fill_color(spl::graphics::color::blue));

        image.draw(spl::graphics::rectangle{{150 + 15, 150 + offset}, 90, 30,  i, true}.fill_color(spl::graphics::color::red));
        image.draw(spl::graphics::rectangle{{150 - 15, 150 - offset}, 90, 30, -i, true}.fill_color(spl::graphics::color::yellow));
    }

    image.draw(spl::graphics::regular_polygon{{600, 400}, 50, 8, 3.141592653589793 / 5});
    image.draw(spl::graphics::regular_polygon{{550, 500}, 50, 5, 3.141592653589793 / 5}.fill_color(spl::graphics::color::cyan));

    image.draw(spl::graphics::circle{{550, 400}, 45}.border_color(spl::graphics::color::green).fill_color(spl::graphics::color::violet));
    image.draw(spl::graphics::circle{{520, 400}, 45}.border_color(spl::graphics::color::green));

    image.draw(spl::graphics::line{{400, 400}, {403, 403}, 1, spl::graphics::color::black, false});

    for (int j = 0; j < 5; ++j) {
        auto const o = 100 * (j + 1);
        if (j < 3) continue;
        for (int i = 0; i < 11; ++i) {
            auto const th = 2 * 3.141592653589793 * i / 11;
            auto const len = 100;
            auto const off = 7 + 2;
            auto const from = spl::graphics::vertex(o + off * std::cos(th), o + off * std::sin(th));
            auto const to   = spl::graphics::vertex(o + len * std::cos(th), o + len * std::sin(th));
            image.draw(spl::graphics::line{from, to, 7, spl::graphics::color::blue});
        }
        break;
    }
    // image.pixel(400, 392) = spl::graphics::color::green;
    // image.pixel(400, 394) = spl::graphics::color::green;
    // image.pixel(399, 393) = spl::graphics::color::green;
    // image.pixel(401, 393) = spl::graphics::color::green;



    // image.draw(spl::graphics::rectangle{{450, 300}, {50, 20}, 45 * 3.141592 / 180}.fill_color(spl::graphics::color::orange).border_color(spl::graphics::color::blue));

    if (not image.save_to_file("example_graphics.png")) { fmt::print(stderr, "Error - can't save the image to a file\n"); };

    auto status = image.load_from_file("example_graphics.png");
    if (status != spl::graphics::load_status::success) {
        std::cerr << "Error while loading a.png\n";
    }
    image.save_to_file("example_graphics_clone.jpg");
} catch (std::exception & e) {
    fmt::print(stderr, "exception caught: {}\n", e.what());
}
