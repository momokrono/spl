#include "spl/image.hpp"
#include "spl/effects.hpp"
#include "fmt/core.h"

int main()
{
	auto mario = spl::graphics::image{};
	mario.load_from_file("mario.jpg");
	auto mario_bw = spl::graphics::effects::greyscale(mario);
	mario_bw.save_to_file("mario_bw.png");
	{
		auto mario_copy = mario;
		auto const width = mario.width() / 2;
		auto const height = mario.height() / 2;
		auto quadrant_2 = spl::graphics::viewport{mario_copy, 0, 0, width, height};
		auto quadrant_4 = spl::graphics::viewport{mario_copy, mario.swidth() / 2, mario.sheight() / 2, width , height};
		spl::graphics::effects::greyscale(std::in_place, quadrant_2);
		spl::graphics::effects::greyscale(std::in_place, quadrant_4);
		mario_copy.save_to_file("mario_bw_quadrants.png");
	}

	auto mario_blurred = spl::graphics::effects::triangular_blur(mario, 5);
	mario_blurred.save_to_file("mario_tri_blurred.png");

	{
		auto blue_yellow = mario;
		auto const width = mario.width() / 2;
		auto const height = mario.height() / 2;

		blue_yellow.fill(spl::graphics::color::blue);

		auto quadrant_2 = spl::graphics::viewport{blue_yellow, 0, 0, width, height};
		quadrant_2.fill(spl::graphics::color::yellow);
		auto quadrant_4 = spl::graphics::viewport{blue_yellow, mario.swidth() / 2, mario.sheight() / 2, width , height};
		quadrant_4.fill(spl::graphics::color::yellow);
		spl::graphics::effects::triangular_blur(std::in_place, quadrant_2, 35);
		spl::graphics::effects::triangular_blur(std::in_place, quadrant_4, 15);
		blue_yellow.save_to_file("mario_triblur_quadrants.png");
	}
}