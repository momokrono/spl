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

/// Calculates the effective coordinate of the point to select, reflecting them if
/// they exceedes the border
/// \param x x coordinate relative to the image_view
/// \param y y coordinate relative to the image_view
/// \param original the image_view on which the blur is working
///
/// \return the pair with the effective coordinates relative to the base image
auto _effective_coordinates(auto x, auto y, image_view const & original)
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

/// Calculates the color of a pixel using a triangular blur
/// \param x_p x coordinate of the point, relative to the view
/// \param y_p y coordinate of the point, relative to the view
/// \param radius radius of the area of influence for the blur
/// \param view view on the area being blurred
/// \param base_img the base image
/// \return the color of the pixel in `(x_p, y_p)` computed as an average of the colors in a square
///         of side `2 * radius + 1` centered in the pixel, with weight inversely proportional to
///         the distance
auto _triangular_blur_impl(int64_t x_p, int64_t y_p, int16_t radius, image_view view, image const & base_img) noexcept
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
            auto [effective_x, effective_y] = _effective_coordinates(x, y, view);
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

void _triangular_blur(int16_t radius, viewport output, image_view original)
{
    auto const & base_img = original.base();
    for (auto y = 0; y < output.sheight(); ++y) {
        for (auto x = 0; x < output.swidth(); ++x) {
            auto & pixel = output.pixel(x, y);
            pixel = _triangular_blur_impl(x, y, radius, output, base_img);
        }
    }
}

void _box_blur(int16_t radius, viewport output, image_view original)
{
    if (output.height() == 0 or output.width() == 0) {
        return;
    }
    auto const sqrt_norm_factor = 1. / (2 * radius + 1);
    auto [x0, y0] = output.offset();

    auto sum_sq_column = [radius, &original](int_fast32_t col, int_fast32_t y)
    {
        auto [r, g, b, a] = std::array{0, 0, 0, 0};
        for (int_fast32_t t = y - radius; t <= y + radius; ++t) {
            auto [effective_x, effective_y] = _effective_coordinates(col, t, original);
            auto [r_, g_, b_, a_] = original.base().pixel(effective_x, effective_y);
            r += r_ * r_;
            g += g_ * g_;
            b += b_ * b_;
            a += a_;
        }
        return std::array<int_fast32_t, 4>{r, g, b, a};
    };  // in the output frame of reference

    auto sum_sq_row = [radius, &original](int_fast32_t x, int_fast32_t row)
    {
        auto [r, g, b, a] = std::array{0, 0, 0, 0};
        for (int_fast32_t t = x - radius; t <= x + radius; ++t) {
            auto [effective_x, effective_y] = _effective_coordinates(t, row, original);
            auto [r_, g_, b_, _a] = original.base().pixel(effective_x, effective_y);
            r += r_ * r_;
            g += g_ * g_;
            b += b_ * b_;
            a += _a;
        }
        return std::array<int_fast32_t, 4>{r, g, b, a};
    };  // in the output frame of reference


    // First avg computation
    auto [r, g, b, a] = std::array{0, 0, 0, 0}; // y_avg;
    for (int t = -radius; t <= radius; ++t) {
        auto const [r_, g_, b_, a_] = sum_sq_column(x0 + t, y0);
        r += r_;
        g += g_;
        b += b_;
        a += a_;
    }

    auto y = 0;
    while (true) {
        auto x_r = r;
        auto x_g = g;
        auto x_b = b;
        auto x_a = a;

        auto x = 0;
        while (true) {
            output.pixel(x, y) = rgba{
                static_cast<uint8_t>(std::sqrt(x_r) * sqrt_norm_factor),
                static_cast<uint8_t>(std::sqrt(x_g) * sqrt_norm_factor),
                static_cast<uint8_t>(std::sqrt(x_b) * sqrt_norm_factor),
                static_cast<uint8_t>(a * sqrt_norm_factor * sqrt_norm_factor)
            };
            ++x;
            if (x >= output.swidth()) {
                break;
            }
            auto const [prev_r, prev_g, prev_b, prev_a] = sum_sq_column(x + x0 - radius - 1, y + y0);
            auto const [next_r, next_g, next_b, next_a] = sum_sq_column(x + x0 + radius, y + y0);
            x_r += next_r - prev_r;
            x_g += next_g - prev_g;
            x_b += next_b - prev_b;
            x_a += next_a - prev_a;
        }
        ++y;
        if (y >= output.sheight()) {
            break;
        }
        auto const [prev_r, prev_g, prev_b, prev_a] = sum_sq_row(x0, y + y0 - radius - 1);
        auto const [next_r, next_g, next_b, next_a] = sum_sq_row(x0, y + y0 + radius);
        r += next_r - prev_r;
        g += next_g - prev_g;
        b += next_b - prev_b;
        a += next_a - prev_a;
    }
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
        effect_impl(radius, result, base_img);
    } else {
        if (threads <= 0) {
            threads = std::thread::hardware_concurrency();
        }

        auto const num_threads = std::min<uint16_t>(threads, std::thread::hardware_concurrency());
        auto const view_height = result.height() / num_threads;
        auto const remaining = result.height() % num_threads;

        auto workers = std::vector<std::jthread>{};
        workers.reserve(num_threads);

        for (auto i = 0ul; i < num_threads; ++i) {
            auto const start_px = static_cast<int_fast32_t>(view_height * i);
            auto output = spl::graphics::viewport{result, 0, start_px, result.width(), view_height};
            workers.emplace_back(effect_impl, radius, output, image_view{base_img});
        }
        effect_impl(radius, viewport{
            result, 0, static_cast<int_fast32_t>(view_height * num_threads), result.width(), remaining
        }, base_img);
    }
}

} // namespace spl::graphics
