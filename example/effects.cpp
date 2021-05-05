#include "spl/image.hpp"
#include "spl/effects.hpp"
#include "fmt/core.h"

int main()
{
	auto mario = spl::graphics::image{};
	mario.load_from_file("mario.jpg");
	auto mario_bw = spl::graphics::effects::greyscale(mario);
	mario_bw.save_to_file("mario_bw.jpg");
	spl::graphics::effects::triangular_blur(std::in_place, mario, 5);
	mario.save_to_file("mario_blurred.jpg");
}