#include "spl/effects.hpp"
#include "spl/image.hpp"
#include "fmt/core.h"

auto black_and_white(auto const & image)
{
    constexpr auto result_name = "example_effects_bw.png";
    fmt::print("Black and white ({})...\n", result_name);
    auto image_bw = spl::graphics::greyscale(image);
    image_bw.save_to_file("example_effects_bw.png");
}

auto black_and_white_quadrants(auto image)
{
    constexpr auto result_name = "example_effects_bw_quadrants.png";
    fmt::print("Black and white quadrants ({})...\n", result_name);
    auto const width = image.width() / 2;
    auto const height = image.height() / 2;
    auto quadrant_2 = spl::graphics::viewport{image, 0, 0, width, height};
    auto quadrant_4 = spl::graphics::viewport{image, image.swidth() / 2, image.sheight() / 2, width , height};
    spl::graphics::greyscale(std::in_place, quadrant_2);
    spl::graphics::greyscale(std::in_place, quadrant_4);
    image.save_to_file(result_name);
}

auto blur(auto const & image)
{
    constexpr auto result_name = "example_effects_triangular_blur.png";
    fmt::print("Blur ({})...\n", result_name);
    auto image_blurred = spl::graphics::blur(spl::graphics::effects::triangular_blur, image, 5, 0);
    image_blurred.save_to_file(result_name);
}

auto blur_quadrants(auto const & image)
{
    constexpr auto result_name = "example_effects_blur_quadrants.png";
    fmt::print("Blurred quadrants ({})...\n", result_name);
    auto image_blurred = spl::graphics::blur(spl::graphics::effects::triangular_blur, image, 5, 0);
    image_blurred.save_to_file(result_name);
}

auto blur_colored_quadrants()
{
    constexpr auto result_name = "example_effects_blur_colored_quadrants.png";
    fmt::print("Blur on colored quadrants ({})...\n", result_name);
    auto image = spl::graphics::image{800, 600, spl::graphics::color::blue};
    auto const width = image.width() / 2;
    auto const height = image.height() / 2;

    auto quadrant_2 = spl::graphics::viewport{image, 0, 0, width, height};
    quadrant_2.fill(spl::graphics::color::yellow);
    auto quadrant_4 = spl::graphics::viewport{image, image.swidth() / 2, image.sheight() / 2, width , height};
    quadrant_4.fill(spl::graphics::color::green);
    spl::graphics::blur(std::in_place, spl::graphics::effects::box_blur, quadrant_2, 35, 0);
    spl::graphics::blur(std::in_place, spl::graphics::effects::triangular_blur, quadrant_4, 15, 2);
    image.save_to_file(result_name);
}

int main(int argc, char * argv[])
{
    if (argc != 1) {
        auto const filename = std::string_view{argv[1]};
        auto image = spl::graphics::image{};
        image.load_from_file(filename);

        black_and_white(image);
        black_and_white_quadrants(image);

        blur(image);
        blur_quadrants(image);
    } else {
        fmt::print("No image passed as argument - only colored quadrants will be generated\n");
    }
    blur_colored_quadrants();
}
