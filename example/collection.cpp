/**
 * @author      : @rbrugo, @momokrono
 * @file        : collection
 * @created     : Friday Mar 05, 2021 11:51:30 CET
 * @license     : MIT
 */

#include <fmt/format.h>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <thread>
#include <mutex>

#include "spl/image.hpp"
#include "spl/primitive.hpp"
#include "spl/collection.hpp"

namespace sgl = spl::graphics;

int main() try
{
    // auto image = spl::graphics::image(600,400);
    constexpr auto width  = 10'000;
    constexpr auto height = 10'000;
    auto image = spl::graphics::image(width, height);

    auto const w = width - 1;
    auto const h = height / 2;

    constexpr auto tot_lines = 1'000;
    constexpr auto n_threads = 6;
    constexpr auto lines_per_thread = tot_lines / n_threads;
    auto threads = std::vector<std::thread>{};
    threads.reserve(n_threads);
    auto m = std::mutex{};
    auto collection = spl::graphics::collection{};

    auto build_collection = [&collection, &m](int const w, int const h, int const from, int const to) mutable
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

        auto guard = std::unique_lock{m};
        collection.push(std::move(subcollection));
    };

    for (auto i = 0ul; i < n_threads; ++i) {
        threads.emplace_back(build_collection, w, h, i * lines_per_thread, (i + 1) * lines_per_thread);
    }

    for (auto & thread : threads) {
        thread.join();
    }
    collection.render_on(image);

    if (not image.save_to_file("example_collection.png")) {
        fmt::print(stderr, "Error while trying to save the generate image to file\n");
    }
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}



