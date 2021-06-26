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

// enum class effects { triangular_blur, box_blur, kawase_blur, bloom };
namespace effects {
    // struct grayscale {};
    struct triangular { int_fast32_t radius;};
    struct box { int_fast32_t radius; };
    // struct gaussian { ... };
    // struct kawase { int_fast32_t radius; std::vector<int> boh; };
    // struct bokeh { ... };
}

template <typename Effect>
concept blur_policy = std::same_as<Effect, effects::triangular>
                   or std::same_as<Effect, effects::box>;

inline
void greyscale(std::in_place_t, spl::graphics::viewport v)
{
    std::ranges::transform(v, v.begin(), &spl::graphics::grayscale);
}

[[nodiscard]] inline
spl::graphics::image greyscale(spl::graphics::image_view v)
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

void blur(std::in_place_t, effects::triangular, viewport original, int16_t threads = 1);
void blur(std::in_place_t, effects::box, viewport original, int16_t threads = 1);

template <spl::graphics::blur_policy Effect>
[[nodiscard]] inline
spl::graphics::image blur(Effect effect, image_view v, uint16_t threads = 1)
{
    auto img = spl::graphics::image{v};
    blur(std::in_place, effect, img, threads);
    return img;
}

template <spl::graphics::blur_policy Effect>
[[nodiscard]] inline
spl::graphics::image blur(Effect effect, spl::graphics::image && img, uint16_t threads = 1)
{
    auto res = std::move(img);
    blur(std::in_place, effect, res, threads);
    return res;
}

} // namespace spl::graphics

#endif /* EFFECTS_HPP */
