/**
 * @author      : @rbrugo, @momokrono
 * @file        : collection
 * @created     : Friday Mar 05, 2021 11:51:30 CET
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

namespace sgl = spl::graphics;

auto time_passed(auto start)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
}

int main() try
{
    fmt::print("Launching the program\n");
    auto start = std::chrono::steady_clock::now();
    // auto image = spl::graphics::image(600,400);
    constexpr auto width  = 10'000;
    constexpr auto height = 10'000;
    // auto image = spl::graphics::image(width, height, sgl::color::white);
    auto image = spl::graphics::image(sgl::construct_uninitialized, width, height);
    image.fill(sgl::color::white);
    fmt::print("Image created: {}\n", time_passed(start));

    constexpr auto tot_lines = 1'000;
    constexpr auto n_threads = 6;
    constexpr auto lines_per_thread = tot_lines / n_threads;
    auto jobs = std::vector<std::future<sgl::collection>>{};
    jobs.reserve(n_threads);

    auto collection = spl::graphics::collection{};

    auto build_collection = [](int const w, int const h, int const from, int const to) mutable
    {
        auto subcollection = sgl::collection{};

        for (int j = from; j < to; ++j) {
            auto line1 = spl::graphics::line{{0, h}, {w, h + j * 3}, spl::graphics::color::green};
            auto line2 = spl::graphics::line{{w, h}, {0, h + j * 3}, spl::graphics::color::blue};
            auto line3 = spl::graphics::line{{0, h}, {w, h - j * 3}, spl::graphics::color::green};
            auto line4 = spl::graphics::line{{w, h}, {0, h - j * 3}, spl::graphics::color::blue};

            if (j % 2 == 1) {
                std::swap(line1, line2);
                std::swap(line3, line4);
            }

            subcollection.push(line1)
                      .push(line2)
                      .push(line3)
                      .push(line4);
        }

        return subcollection;
    };

    fmt::print("Launching threads...\n");
    start = std::chrono::steady_clock::now();

    for (auto i = 0ul; i < n_threads; ++i) {
        jobs.push_back(std::async(
            build_collection, width - 1, height / 2, i * lines_per_thread, (i + 1) * lines_per_thread)
        );
    }
    for (auto & subcollection : jobs) {
        collection.push(std::move(subcollection.get()));
    }
    fmt::print("Collection built: {}\n", time_passed(start));
    start = std::chrono::steady_clock::now();
    collection.render_on(image);
    fmt::print("Image rendered: {}\n", time_passed(start));
    start = std::chrono::steady_clock::now();

    if (not image.save_to_file("example_collection.jpg")) {
        fmt::print(stderr, "Error while trying to save the generate image to file\n");
    }
    fmt::print("Image saved: {}\n", time_passed(start));
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}



