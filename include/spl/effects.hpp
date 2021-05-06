/**
 * @author      : rbrugo. momokrono
 * @file        : bench
 * @created     : Monday May 4, 2021 23:33:07 CEST
 * @license     : MIT
 */

#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include <algorithm>
#include <execution>
#include <thread>
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

inline
void triangular_blur(std::in_place_t, spl::graphics::viewport original, int16_t radius)
{
	auto effective_coordinates = [original](auto x, auto y) {
		auto const [x0, y0] = original.offset();
		auto effective = [](auto z, auto z0, auto max_z) {
			if (z0 + z >= max_z) {
				return 2 * (max_z - z0) - z - 1;
			}
			if (z0 + z >= 0) {
				return z;
			}
			return - (z0 + z);
		};
		auto effective_x = effective(x, x0, original.base().swidth());
		auto effective_y = effective(y, y0, original.base().sheight());

		return std::pair{effective_x, effective_y};
	};

	auto compute_blur = [&, radius](int64_t x0, int64_t y0, spl::graphics::viewport const & img) noexcept
    	-> spl::graphics::rgba
	{
		auto triangular_filter = [=](int64_t dx, int64_t dy) -> double {
			if (dx + dy < radius) {
				return (1 - (dx + dy) * 1. / radius);
			} else {
				return 0;
			}
		};

		auto total_contribution = 0.;

		double r = 0;
		double g = 0;
		double b = 0;

		for (auto y = y0 - radius; y < y0 + radius; ++y) {
			auto partial_r = 0.;
			auto partial_g = 0.;
			auto partial_b = 0.;
			for (auto x = x0 - radius; x < x0 + radius; ++x) {
				auto weight = triangular_filter(std::abs(x - x0), std::abs(y - y0));
				auto [effective_x, effective_y] = effective_coordinates(x, y);
				auto color = img.pixel(effective_x, effective_y);

				partial_r += weight * color.r * color.r;
				partial_g += weight * color.g * color.g;
				partial_b += weight * color.b * color.b;
				total_contribution += weight;
			}

			r += partial_r;
			g += partial_g;
			b += partial_b;
		}

		auto const norm_factor = 1. / total_contribution;
		r *= norm_factor;
		g *= norm_factor;
		b *= norm_factor;

		return spl::graphics::rgba(std::sqrt(r), std::sqrt(g), std::sqrt(b));
	};
	auto buffer = original;
	/*for (auto x = 0; x < original.swidth(); ++x) {
		for (auto y = 0; y < original.sheight(); ++y) {
			original.pixel(x, y) = compute_blur(x, y, buffer);
		}
	}*/

	/*auto indexes = std::views::iota(0, original.swidth() * original.sheight())
				 | std::views::transform([width = original.swidth()](auto n) { return std::pair{n % width, n / width}; })
				 | std::views::common
				 ;

	std::transform(std::execution::par, indexes.begin(), indexes.end(), buffer.begin(), [=] (auto coord) {
		auto [x, y] = coord;
		return compute_blur(x, y, buffer);
	});*/

	auto const num_threads = std::thread::hardware_concurrency();
	auto const view_height = original.height() / num_threads;
	[[maybe_unused]] auto const remaining = original.sheight() - view_height * num_threads;

	auto workers = std::vector<std::jthread>{};

	auto blur_viewport = [&](spl::graphics::viewport const v) {
		auto[x0, y0] = v.offset();
		for (auto y = 0; y < v.sheight(); ++y) {
			if (y + y0 >= original.sheight()) {
				break;
			}
			for (auto x = 0; x < v.swidth(); ++x) {
				if (x + x0 >= original.swidth()) {
					break;
				}
				original.pixel(x + x0, y + y0) = compute_blur(x, y, v);
			}
		}
	};


	for (auto i = 0ul; i < num_threads; ++i)
	{
		auto v = spl::graphics::viewport{buffer, 0, static_cast<int_fast32_t>(view_height * i), buffer.width() - 1, view_height - 1};
		workers.emplace_back(blur_viewport, v);
	}

//    auto job_for_thread = original.sheight()*original.swidth()/num_threads;
//
//    auto pixels_to_compute = [&](int index)
//		-> void
//	{
//    	for (auto i = job_for_thread*index; i < job_for_thread*(index+1); ++i)
//
//    };


}

[[nodiscard]] inline
spl::graphics::image triangular_blur(spl::graphics::viewport v, int16_t r)
{
	auto img = spl::graphics::image{v};
	triangular_blur(std::in_place, img, r);
	return img;
}

[[nodiscard]] inline
spl::graphics::image triangular_blur(spl::graphics::image && img, int16_t r)
{
	auto res = std::move(img);
	triangular_blur(std::in_place, res, r);
	return res;
}
 /* FIXME : loopa e non esce
void box_blur(std::in_place_t, spl::graphics::viewport original, int16_t radius) {
	auto compute_blur = [](int64_t x0, int64_t y0, spl::graphics::image const &img, int16_t radius) noexcept {
		radius = std::min({x0, y0, img.swidth() - x0, img.sheight() - y0, radius + 0l});
		if (radius == 0) { radius = 1; }
		double r = 0;
		double g = 0;
		double b = 0;
		auto const norm_factor = 1. / ((2 * radius + 1) * (2 * radius + 1));
		auto const max_x = std::min(img.swidth(), x0 + radius + 1);
		auto const max_y = std::min(img.sheight(), y0 + radius + 1);
		for (auto y = std::max(0l, y0 - radius); y < max_y; ++y) {
			auto partial_r = 0.;
			auto partial_g = 0.;
			auto partial_b = 0.;
			for (auto x = std::max(0l, x0 - radius); x < max_x; ++x) {
				auto color = img.pixel(x, y);

				partial_r += color.r * color.r;
				partial_g += color.g * color.g;
				partial_b += color.b * color.b;
			}
			r += partial_r * norm_factor;
			g += partial_g * norm_factor;
			b += partial_b * norm_factor;
		}
		return spl::graphics::rgba(std::sqrt(r), std::sqrt(g), std::sqrt(b));
	};
	auto buffer = original;
	for (auto x = 0; x < original.swidth(); ++x) {
		for (auto y = 0; y < original.sheight(); ++y) {
			original.pixel(x, y) = compute_blur(x, y, buffer, radius);
		}
	}
}

[[nodiscard]]
spl::graphics::image box_blur(spl::graphics::viewport v, int16_t r)
{
	auto img = spl::graphics::image{v};
	box_blur(std::in_place, img, r);
	return img;
}

[[nodiscard]]
spl::graphics::image box_blur(viewport && img, int16_t r)
{
	auto res = std::move(img);
	box_blur(std::in_place, res, r);
	return res;
}
*/
} // namespace spl::graphics::effects

#endif /* EFFECTS_HPP */