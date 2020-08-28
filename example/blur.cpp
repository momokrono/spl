/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * @file        : convolution
 * @created     : Thursday Aug 27, 2020 21:43:48 CEST
 * @license     : MIT
 */

#include <spl/image.hpp>
#include <spl/primitive.hpp>
#include <fmt/format.h>

spl::graphics::rgba blur(int64_t x0, int64_t y0, spl::graphics::image const & img, uint16_t radius = -1)
{
    auto filter = [=, rad = radius > 0 ? radius : 3](int64_t dx, int64_t dy) -> double {
        if (dx + dy < radius) {
            return (1 - (dx + dy) * 1. / radius);
        } else {
            return 0;
        }
    };

    double r = 0;
    double g = 0;
    double b = 0;
    auto const max_x = std::min(img.swidth(), x0 + radius);
    auto const max_y = std::min(img.sheight(), y0 + radius);
    for (auto y = std::max(0l, y0 - radius); y < max_y; ++y) {
        for (auto x = std::max(0l, x0 - radius); x < max_x; ++x) {
            auto weight = filter(std::abs(x - x0), std::abs(y - y0));
            auto color  = img.pixel(x, y);

            r += weight * color.r;
            g += weight * color.g;
            b += weight * color.b;
        }
    }

    auto const norm = 3./19;

    return spl::graphics::rgba(r * norm, g * norm, b * norm);
}

int main(int argc, char * argv[])
{
    auto original = spl::graphics::image{};
    if (argc > 1) {
        if (not std::filesystem::exists(argv[1])) {
            fmt::print(stderr, "File not found: {}\n", argv[1]);
            return 0;
        }
        fmt::print(stderr, "Image {} loaded\n", argv[1]);
        original = spl::graphics::image{}; original.load_from_file(argv[1]);
    } else {
        fmt::print(stderr, "Using a default image\n");
        original = spl::graphics::image{20, 20, spl::graphics::color::white};
        std::ranges::fill(original.column( 8), spl::graphics::color::black);
        std::ranges::fill(original.column( 9), spl::graphics::color::black);
        std::ranges::fill(original.column( 10), spl::graphics::color::black);
        original.draw(spl::graphics::circle{{9, 9}, 5}.border_color(spl::graphics::color::red));
        original.draw(spl::graphics::circle{{9, 9}, 6}.border_color(spl::graphics::color::red));
    }

    original.save_to_file("original.bmp");

    auto blurred1 = spl::graphics::image{spl::graphics::construct_uninitialized, original.width(), original.height()};

    for (auto x = 0; x < blurred1.swidth(); ++x) {
        for (auto y = 0; y < blurred1.sheight(); ++y) {
            blurred1.pixel(x, y) = blur(x, y, original, 3);
        }
    }

    blurred1.save_to_file("blurred_rad_3.bmp");

    auto blurred2 = spl::graphics::image{spl::graphics::construct_uninitialized, original.width(), original.height()};

    for (auto x = 0; x < blurred2.swidth(); ++x) {
        for (auto y = 0; y < blurred2.sheight(); ++y) {
            blurred2.pixel(x, y) = blur(x, y, original, 15);
        }
    }

    blurred1.save_to_file("blurred_rad_15.bmp");
}


