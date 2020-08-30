/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * @file        : convolution
 * @created     : Thursday Aug 27, 2020 21:43:48 CEST
 * @license     : MIT
 */

#include <spl/image.hpp>
#include <spl/primitive.hpp>
#include <fmt/format.h>

auto triangular_blur(int64_t x0, int64_t y0, spl::graphics::image const & img, uint16_t radius) noexcept
    -> spl::graphics::rgba
{
    auto triangular_filter = [=](int64_t dx, int64_t dy) -> double {
        if (dx + dy < radius) {
            return (1 - (dx + dy) * 1. / radius);
        } else {
            return 0;
        }
    };

    constexpr
    auto total_contribution = [](auto const radius) {
        auto acc = 1. + radius * (radius % 2 == 0);
        for (int n = 1, up = std::ceil(radius / 2); n < up; ++n) {
            acc += n * (radius - n);
        }

        return acc;
    };
    static_assert(total_contribution(3) - 3./19 > -0.000000001);
    static_assert(total_contribution(4) - 11    <  0.000000001);

    auto const norm_factor = 1. / total_contribution(radius); //3./19;

    double r = 0;
    double g = 0;
    double b = 0;
    auto const max_x = std::min(img.swidth(), x0 + radius);
    auto const max_y = std::min(img.sheight(), y0 + radius);
    for (auto y = std::max(0l, y0 - radius); y < max_y; ++y) {
        auto partial_r = 0.;
        auto partial_g = 0.;
        auto partial_b = 0.;
        for (auto x = std::max(0l, x0 - radius); x < max_x; ++x) {
            auto weight = triangular_filter(std::abs(x - x0), std::abs(y - y0));
            auto color  = img.pixel(x, y);

            partial_r += weight * color.r;
            partial_g += weight * color.g;
            partial_b += weight * color.b;
        }

        r += partial_r * norm_factor;
        g += partial_g * norm_factor;
        b += partial_b * norm_factor;
    }

    return spl::graphics::rgba(r, g, b);
}

auto box_blur(int64_t x0, int64_t y0, spl::graphics::image & img, uint16_t radius) noexcept
{
    double r = 0;
    double g = 0;
    double b = 0;
    auto const norm_factor = 1. / (4 * radius * radius);
    auto const max_x = std::min(img.swidth(), x0 + radius);
    auto const max_y = std::min(img.sheight(), y0 + radius);
    for (auto y = std::max(0l, y0 - radius); y < max_y; ++y) {
        auto partial_r = 0.;
        auto partial_g = 0.;
        auto partial_b = 0.;
        for (auto x = std::max(0l, x0 - radius); x < max_x; ++x) {
            auto color  = img.pixel(x, y);

            partial_r += color.r;
            partial_g += color.g;
            partial_b += color.b;
        }
        r += partial_r * norm_factor;
        g += partial_g * norm_factor;
        b += partial_b * norm_factor;
    }

    return spl::graphics::rgba(r, g, b);
}

#include <thread>

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

    auto blur_image = [&original](std::string_view const name, auto radius, auto && blur_function) {
        auto blurred = spl::graphics::image{spl::graphics::construct_uninitialized, original.width(), original.height()};

        for (auto x = 0; x < blurred.swidth(); ++x) {
            for (auto y = 0; y < blurred.sheight(); ++y) {
                blurred.pixel(x, y) = blur_function(x, y, original, radius);
            }
        }

        blurred.save_to_file(fmt::format("{}_rad_{}.bmp", name, radius));
        fmt::print("{} rad {} generated\n", name, radius);
    };

    auto const triangular_1 = std::jthread{blur_image, "triangular", 3,  triangular_blur};
    auto const triangular_2 = std::jthread{blur_image, "triangular", 15, triangular_blur};
    auto const box_1        = std::jthread{blur_image, "box",        3,  box_blur};
    auto const box_2        = std::jthread{blur_image, "box",        15, box_blur};
}


