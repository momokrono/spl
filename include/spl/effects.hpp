/**
 * @author      : rbrugo, momokrono
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

void triangular_blur(std::in_place_t, spl::graphics::viewport original, int16_t radius, uint16_t threads = 1);

[[nodiscard]] inline
spl::graphics::image triangular_blur(spl::graphics::viewport v, int16_t r, uint16_t threads = 1)
{
    auto img = spl::graphics::image{v};
    triangular_blur(std::in_place, img, r, threads);
    return img;
}

[[nodiscard]] inline
spl::graphics::image triangular_blur(spl::graphics::image && img, int16_t r, uint16_t threads = 1)
{
    auto res = std::move(img);
    triangular_blur(std::in_place, res, r, threads);
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
