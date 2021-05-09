/**
 * @author      : rbrugo, momokrono
 * @file        : effects.cpp
 * @created     : Monday May 4, 2021 23:33:07 CEST
 * @license     : MIT
 */

#include "spl/effects.hpp"
#include "fmt/core.h"
#include <thread>

namespace spl::graphics
{

auto _effective_coordinates(auto x, auto y, auto const & original)
{
    auto effective = [](auto z, auto z0, auto max_z) {
        if (z0 + z >= max_z) {
            return 2 * max_z - (z + z0) - 1;
        }
        if (z0 + z >= 0) {
            return z0 + z;
        }
        return -z;
    };

    auto const [x0, y0] = original.offset();
    auto effective_x = effective(x, x0, original.base().swidth());
    auto effective_y = effective(y, y0, original.base().sheight());

    return std::pair{effective_x, effective_y};
}

auto _triangular_blur(int64_t x_p, int64_t y_p, int16_t radius, image_view original, image const & base_img) noexcept
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
    auto [r, g, b] = std::tuple{0., 0., 0.};

    for (auto y = y_p - radius; y < y_p + radius; ++y) {
        auto partial_r = 0.;
        auto partial_g = 0.;
        auto partial_b = 0.;
        for (auto x = x_p - radius; x < x_p + radius; ++x) {
            auto weight = triangular_filter(std::abs(x - x_p), std::abs(y - y_p));
            auto [effective_x, effective_y] = _effective_coordinates(x, y, original);
            auto color = base_img.pixel(effective_x, effective_y);

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

    return spl::graphics::rgba{
        static_cast<uint8_t>(std::sqrt(r)), static_cast<uint8_t>(std::sqrt(g)), static_cast<uint8_t>(std::sqrt(b))
    };
}

auto _box_blur(int64_t x0, int64_t y0, int16_t radius, image_view original, image const & base_img) noexcept
{
    double r = 0;
    double g = 0;
    double b = 0;
    auto const norm_factor = 1. / ((2 * radius + 1) * (2 * radius + 1));
    for (auto y = y0 - radius; y < y0 + radius; ++y) {
        auto partial_r = 0.;
        auto partial_g = 0.;
        auto partial_b = 0.;
        for (auto x = x0 - radius; x < x0 + radius; ++x) {
            auto [effective_x, effective_y] = _effective_coordinates(x, y, original);
            auto color = base_img.pixel(effective_x, effective_y);

            partial_r += color.r * color.r;
            partial_g += color.g * color.g;
            partial_b += color.b * color.b;
        }
        r += partial_r * norm_factor;
        g += partial_g * norm_factor;
        b += partial_b * norm_factor;
    }
    return spl::graphics::rgba(std::sqrt(r), std::sqrt(g), std::sqrt(b));
}

void blur(std::in_place_t, effects effect, viewport result, int16_t radius, int16_t threads)
{
   auto const effect_impl = [effect] {
       switch (effect) {
       case effects::triangular_blur:
           return _triangular_blur;
       case effects::box_blur:
           return _box_blur;
           // case effects::kawase_blur: return _kawase_blur
       default:
           fmt::print("Unrecognized blur type, exiting.\n");
           std::exit(1); // TODO: exit numbers
           // TODO: exception? boh
       }
   }();

   auto const base_img = result.base();

    if (threads == 1) {
        for (auto x = 0; x < result.swidth(); ++x) {
            for (auto y = 0; y < result.sheight(); ++y) {
                result.pixel(x, y) = effect_impl(x, y, radius, result, base_img);
            }
        }
    } else {
        if (threads <= 0) {
            threads = std::thread::hardware_concurrency();
        }
        auto const num_threads = std::min<uint16_t>(threads, std::thread::hardware_concurrency());
        auto const view_height = result.height() / num_threads;
        auto const remaining = result.height() % num_threads;

        auto workers = std::vector<std::jthread>{};
        workers.reserve(num_threads);

        auto apply_effect = [&](spl::graphics::image_view const original) {
            auto const [x0, y0] = original.offset();
            for (auto y = 0; y < original.sheight(); ++y) {
                if (y + y0 >= result.sheight()) {
                    break;
                }
                for (auto x = 0; x < original.swidth(); ++x) {
                    if (x + x0 >= result.swidth()) {
                        break;
                    }
                    result.pixel(x + x0, y + y0) = effect_impl(x, y, radius, original, base_img);
                }
            }
        };

        for (auto i = 0ul; i < num_threads; ++i) {
            auto const start_px = static_cast<int_fast32_t>(view_height * i);
            auto const v = spl::graphics::image_view{base_img, 0, start_px, base_img.width(), view_height};
            workers.emplace_back(apply_effect, v);
        }
        apply_effect(spl::graphics::image_view{
            base_img, 0, static_cast<int_fast32_t>(view_height * num_threads), base_img.width(), remaining
        });
    }
}

} // namespace spl::graphics
