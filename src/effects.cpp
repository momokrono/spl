/**
 * @author      : rbrugo, momokrono
 * @file        : bench
 * @created     : Monday May 4, 2021 23:33:07 CEST
 * @license     : MIT
 */

#include "spl/effects.hpp"

namespace spl::graphics::effects
{

auto _effective_coordinates(auto x, auto y, auto const & original)
{
    auto const [x0, y0] = original.offset();
    auto effective = [](auto z, auto z0, auto max_z) {
        if (z0 + z >= max_z) {
            return 2 * max_z - (z + z0) - 1;
        }
        if (z0 + z >= 0) {
            return z0 + z;
        }
        return - z;
    };
    auto effective_x = effective(x, x0, original.base().swidth());
    auto effective_y = effective(y, y0, original.base().sheight());

    return std::pair{effective_x, effective_y};
}

void triangular_blur(std::in_place_t, spl::graphics::viewport original, int16_t radius, uint16_t threads)
{

    auto compute_blur = [&original, radius](int64_t x_p, int64_t y_p, image const & base_img) noexcept
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

        return spl::graphics::rgba(std::sqrt(r), std::sqrt(g), std::sqrt(b));
    };
    auto const base_img = original.base();

    if (threads == 1) {
        for (auto x = 0; x < original.swidth(); ++x) {
            for (auto y = 0; y < original.sheight(); ++y) {
                original.pixel(x, y) = compute_blur(x, y, base_img);
            }
        }
    } else {
        if (threads <= 0) {
            threads = std::thread::hardware_concurrency();
        }
        auto const num_threads = std::min<uint16_t>(threads, std::thread::hardware_concurrency());
        auto const view_height = original.height() / num_threads;
        auto const remaining = original.sheight() - view_height * num_threads;

        auto workers = std::vector<std::jthread>{};

        auto blur_viewport = [&](spl::graphics::image_view const v) {
            auto const [x0, y0] = v.offset();
            for (auto y = 0; y < v.sheight(); ++y) {
                if (y + y0 >= original.sheight()) {
                    break;
                }
                for (auto x = 0; x < v.swidth(); ++x) {
                    if (x + x0 >= original.swidth()) {
                        break;
                    }
                    original.pixel(x + x0, y + y0) = compute_blur(x + x0, y + y0, v.base());
                }
            }
        };


        for (auto i = 0ul; i < num_threads; ++i) {
            auto v = spl::graphics::image_view{
                base_img, 0, static_cast<int_fast32_t>(view_height * i), base_img.width(), view_height
            };
            workers.emplace_back(blur_viewport, v);
        }
        blur_viewport(spl::graphics::image_view{
            base_img, 0, static_cast<int_fast32_t>(view_height * num_threads), base_img.width(), remaining
        });
    }
}
} // namespace spl::graphics::effects

// bloom
// blur
