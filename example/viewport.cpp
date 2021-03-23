/**
 * @author      : rbrugo, momokrono
 * @file        : viewport
 * @created     : Sunday Mar 21, 2021 23:33:28 CET
 * @license     : MIT
 */

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <future>
#include <mutex>

#include <chrono>

#include "spl/image.hpp"
#include "spl/primitive.hpp"
#include "spl/collection.hpp"
#include "spl/viewport.hpp"

namespace sgl = spl::graphics;

auto build_face(int w, int h)
{
    auto group = sgl::collection{};
    group.push(sgl::circle({int(0.25 * w), int(0.2 * h)}, 0.25 * 0.5 * w).fill_color(sgl::color::black));
    group.push(sgl::circle({int(0.75 * w), int(0.2 * h)}, 0.25 * 0.5 * w).fill_color(sgl::color::black));

    group.push(sgl::line({int(0.5 * w), int(0.2 * h)}, {int(1.5 * 0.25 * w), int(2.4 * 0.2 * h)}));
    group.push(sgl::line({int(1.5 * 0.25 * w), int(2.4 * 0.2 * h)}, {int(2.5 * 0.25 * w), int(2.4 * 0.2 * h)}));

    group.push(sgl::line({int(0.9 * 0.25 * w), int(0.2 * 3 * h)}, {int(3.1 * 0.25 * w), int(0.2 * 3 * h)}));
    group.push(sgl::line({int(0.9 * 0.25 * w), int(0.2 * 4 * h)}, {int(3.1 * 0.25 * w), int(0.2 * 4 * h)}));
    group.push(sgl::line({int(0.9 * 0.25 * w), int(0.2 * 3 * h)}, {int(0.9 * 0.25 * w), int(0.2 * 4 * h)}));
    group.push(sgl::line({int(3.1 * 0.25 * w), int(0.2 * 3 * h)}, {int(3.1 * 0.25 * w), int(0.2 * 4 * h)}));

    return group;
}

int main()
{
    constexpr auto width  = 800;
    constexpr auto height = 600;
    auto image = sgl::image(width, height, sgl::color::white);

    auto group = sgl::collection{};
    {
        auto circle = sgl::circle{{100, 100}, 50};
        circle.fill_color(sgl::color::red);
        auto line1  = sgl::line{{50, 50}, {150, 150}, sgl::color::blue};
        auto line2  = sgl::line{{150, 50}, {50, 150}, sgl::color::green};

        group.push(circle).push(line1).push(line2);
    }

    for (auto y = 0ul; y + 150 < height; y += 100) {
        for (auto x = 0ul; x + 150 < width; x += 100) {
            auto viewport = sgl::viewport{image, x, y, 151, 151};
            group.render_on(viewport);
        }
    }

    {
        auto face = build_face(400, 500);
        auto viewport = sgl::viewport{image, 200, 50, 401, 501};
        face.render_on(viewport);
    }

    image.save_to_file("test_viewport.png");
}

