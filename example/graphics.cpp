#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>

#include "graphics/image.hpp"
#include "graphics/primitive.hpp"

int main() try
{
    auto image = spl::graphics::image(300,200);
    auto col_1 = image.column(0);
    auto row_1 = image.row(0);
    image.fill({0, 255, 255, 128 * 2 - 3});
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

    /* static_assert(spl::detail::has_render_on_member_function<spl::graphics::line>); */
    image.draw(spl::graphics::line{{0, 0}, {100, 199}, spl::graphics::color::red});
    image.draw(spl::graphics::line{{0, 0}, {299,  50}, spl::graphics::color::blue});

    image.draw(spl::graphics::rectangle{{50, 50}, {100, 50}});
    image.draw(spl::graphics::rectangle{{50, 50}, {50, 20}, 3.1415926535 * 0.33, true}.fill_color(spl::graphics::color::red));

    if (not image.save_to_file("a.png")) { fmt::print("NON VA\n"); };
    // image.save_to_file("a.bmp");
    // image.save_to_file("a.jpg");
    // image.save_to_file("a.ppm");
    // image.save_to_file("a.pam");

    auto status = image.load_from_file("a.bmp");
    if (status != spl::graphics::load_status::success) {
        std::cerr << "Errore\n";
    }
    image.save_to_file("a_clone.png");

    // image.pixel(88, 99);
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}
