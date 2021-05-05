/**
 * @author      : rbrugo. momokrono
 * @file        : bench
 * @created     : Monday May 4, 2021 23:33:07 CEST
 * @license     : MIT
 */

#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include <algorithm>
#include <ranges>

#include "spl/viewport.hpp"
#include "spl/image.hpp"

namespace spl::graphics::effects
{
// blur
// bloom
// grayscale

inline
void greyscale(std::in_place_t, spl::graphics::viewport v)
{
	std::ranges::transform(v, v.begin(), &spl::graphics::grayscale);
}

[[nodiscard]] inline
spl::graphics::image greyscale(spl::graphics::viewport v)
{
	auto img = spl::graphics::image{v};
	greyscale(std::in_place, img);
	return img;
}

[[nodiscard]] inline
spl::graphics::image greyscale(image && img)
{
	auto res = std::move(img);
	greyscale(std::in_place, res);
	return res;
}

} // namespace spl::graphics::effects

#endif /* EFFECTS_HPP */