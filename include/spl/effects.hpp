/**
 * @author      : rbrugo, momokrono
 * @file        : effects.hpp
 * @created     : Monday May 4, 2021 23:33:07 CEST
 * @license     : MIT
 */

#ifndef EFFECTS_HPP
#define EFFECTS_HPP

#include <algorithm>

#include "spl/viewport.hpp"
#include "spl/image.hpp"

namespace spl::graphics
{
// blur
// bloom
// grayscale

enum class effects { triangular_blur, box_blur, kawase_blur, bloom };

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

// TODO: order of arguments
void blur(std::in_place_t, effects effect, viewport original, int16_t radius, int16_t threads = 1);

[[nodiscard]] inline
spl::graphics::image blur(effects effect, image_view v, int16_t radius, uint16_t threads = 1)
{
    auto img = spl::graphics::image{v};
    blur(std::in_place, effect, img, radius, threads);
    return img;
}

[[nodiscard]] inline
spl::graphics::image blur(effects effect, spl::graphics::image && img, int16_t radius, uint16_t threads = 1)
{
    auto res = std::move(img);
    blur(std::in_place, effect, res, radius, threads);
    return res;
}

} // namespace spl::graphics

#endif /* EFFECTS_HPP */
