#include "spl/effects.hpp"
#include "spl/image.hpp"
#include "fmt/core.h"

int main(int argc, char * argv[])
{
    if (argc == 1) {
        fmt::print("Usage: {} <filename>\n", argv[0]);
        return 0;
    }
    auto const filename = std::string_view{argv[1]};
    auto image = spl::graphics::image{};
    image.load_from_file(filename);
    auto image_bw = spl::graphics::greyscale(image);
    image_bw.save_to_file("example_effects_bw.png");
    {
        auto image_copy = image;
        auto const width = image.width() / 2;
        auto const height = image.height() / 2;
        auto quadrant_2 = spl::graphics::viewport{image_copy, 0, 0, width, height};
        auto quadrant_4 = spl::graphics::viewport{image_copy, image.swidth() / 2, image.sheight() / 2, width , height};
        spl::graphics::greyscale(std::in_place, quadrant_2);
        spl::graphics::greyscale(std::in_place, quadrant_4);
        image_copy.save_to_file("example_effects_bw_quadrants.png");
    }


    auto image_blurred = spl::graphics::blur(spl::graphics::effects::triangular_blur, image, 5, 0);
    image_blurred.save_to_file("example_effects_triangular_blur.png");

    {
        auto blue_yellow = image;
        auto const width = image.width() / 2;
        auto const height = image.height() / 2;

        blue_yellow.fill(spl::graphics::color::blue);

        auto quadrant_2 = spl::graphics::viewport{blue_yellow, 0, 0, width, height};
        quadrant_2.fill(spl::graphics::color::yellow);
        auto quadrant_4 = spl::graphics::viewport{blue_yellow, image.swidth() / 2, image.sheight() / 2, width , height};
        quadrant_4.fill(spl::graphics::color::yellow);
        spl::graphics::blur(std::in_place, spl::graphics::effects::box_blur, quadrant_2, 35, 0);
        spl::graphics::blur(std::in_place, spl::graphics::effects::triangular_blur, quadrant_4, 15, 0);
        blue_yellow.save_to_file("example_effects_tri_box_blur_quadrants.png");
    }
}
