#include "spl/image.hpp"
#include "spl/effects.hpp"
#include "fmt/core.h"

int main()
{
	auto mario = spl::graphics::image{};
	mario.load_from_file("mario.jpg");
	spl::graphics::effects::greyscale(std::in_place, mario);
	mario.save_to_file("mario_bw.jpg");
}