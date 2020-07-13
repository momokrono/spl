/**
 * @author      : rbrugo, momokrono
 * @file        : primitive
 * @created     : Monday Jul 13, 2020 02:31:15 CEST
 * @license     : MIT
 */

#include "graphics/primitive.hpp"

namespace spl::graphics
{

void line::render_on(image & img)
{
    if (anti_aliasing) {
        draw_antialiased(img);
    }
    else {
        draw_aliased(img);
    }
}

void line::draw_antialiased_parametric(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    if (x1 == x2) {
        auto [from, to] = std::minmax({y1, y2});

        for (; from <= to; ++from) {
            auto & pixel = img.pixel(x1, from);
            pixel = over(color, pixel);
        }
    } else if (y1 == y2) {
        auto [from, to] = std::minmax({x1, x2});

        for (; from <= to; ++from) {
            auto & pixel = img.pixel(from, y1);
            pixel = over(color, pixel);
        }
    } else {
        if (x2 < x1) {
            std::swap(x2, x1);
            std::swap(y2, y1);
        }
        auto const dx = std::abs(x2 - x1);
        auto const dy = std::abs(y2 - y1);
        auto const distance = dx + dy;

        auto const kx = dx * 1.f / distance;
        auto const ky = dy * 1.f / distance * ((y2 > y1) * 2 - 1);
        for (int_fast32_t t = 0; t < distance; ++t) {
            auto const x = x1 + t * kx;
            auto const y = y1 + t * ky;

            for (auto const rounded_y : { std::floor(y), std::ceil(y) }) {
                auto const y_blend = 1.f - std::abs(y - rounded_y);
                for (auto const rounded_x : { std::floor(x), std::ceil(x) }) {
                    auto const x_blend = 1.f - std::abs(x - rounded_x);

                    auto & pixel = img.pixel(rounded_x, rounded_y);
                    pixel = over(color.blend(y_blend * x_blend), pixel);
                }
            }
        }
        // one last time for the final pixel
        {
            auto const t = distance;
            auto const x = x1 + t * kx;
            auto const y = y1 + t * ky;

            auto const y_blend = 1.f - std::abs(y - std::floor(y));
            auto const x_blend = 1.f - std::abs(x - std::floor(x));

            auto & pixel = img.pixel(std::floor(x), std::floor(y));
            pixel = over(color.blend(y_blend * x_blend), pixel);
        }
    }
}

void line::draw_aliased(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    if (x1 == x2) {
        auto [from, to] = std::minmax({y1, y2});

        for (; from <= to; ++from) {
            img.pixel(x1, from) = color;
        }
    } else if (y1 == y2) {
        auto [from, to] = std::minmax({x1, x2});

        for (; from <= to; ++from) {
            img.pixel(from, y1) = color;
        }
    } else if (std::abs(x2 - x1) >= std::abs(y2 - y1)){
        auto const m = (y2 - y1) * 1.f / (x2 - x1);
        auto const q = y2 - m * x2;

        auto [from, to] = std::minmax({x1, x2});
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            auto const y = m * from + q;
            if (y >= 0 and y < img.height()) {
                img.pixel(from, std::lround(y)) = color;
            }
        }
    } else {
        auto const m_rev = (x2 - x1) * 1.f / (y2 - y1);
        auto const q = y2 - x2 / m_rev;

        auto [from, to] = std::minmax({y1, y2});
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            // y = mx + q
            // x = (y - q)/m
            auto const x = (from - q) * m_rev;
            if (x >= 0 and x < img.width()) {
                img.pixel(std::lround(x), from) = color;
            }
        }
    }
}

void line::draw_antialiased(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    if (x1 == x2) {
        auto [from, to] = std::minmax({y1, y2});

        for (; from <= to; ++from) {
            img.pixel(x1, from) = color;
        }
    } else if (y1 == y2) {
        auto [from, to] = std::minmax({x1, x2});

        for (; from <= to; ++from) {
            img.pixel(from, y1) = color;
        }
    } else if (std::abs(x2 - x1) >= std::abs(y2 - y1)){
        auto const m = (y2 - y1) * 1.f / (x2 - x1);
        auto const q = y2 - m * x2;

        auto [from, to] = std::minmax({x1, x2});
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            auto const y = m * from + q;
            if (auto const fy = std::floor(y); fy >= 0.f and fy < img.height()) {
                auto & pixel = img.pixel(from, static_cast<int_fast32_t>(fy));
                pixel = over(color.blend(1. - std::abs(y - fy)), pixel);
            }
            if (auto const cy = std::ceil(y); cy >= 0.f and cy < img.height()) {
                auto & pixel = img.pixel(from, static_cast<int_fast32_t>(cy));
                pixel = over(color.blend(1. - std::abs(y - cy)), pixel);;
            }
        }
    } else {
        auto const m_rev = (x2 - x1) * 1.f / (y2 - y1);
        auto const q = y2 - x2 / m_rev;

        auto [from, to] = std::minmax({y1, y2});
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            auto const x = (from - q) * m_rev;
            if (auto const fx = std::floor(x); fx >= 0 and fx < img.width()) {
                auto & pixel = img.pixel(static_cast<int_fast32_t>(fx), from);
                pixel = over(color.blend(1. - std::abs(x - fx)), pixel);
            }
            if (auto const cx = std::ceil(x); cx >= 0 and cx < img.width()) {
                auto & pixel = img.pixel(static_cast<int_fast32_t>(cx), from);
                pixel = over(color.blend(1. - std::abs(x - cx)), pixel);
            }
        }
    }
}

void rectangle::render_on(image & img)
{
    auto const sin = std::sin(_rotation);
    auto const cos = std::cos(_rotation);

    auto const x1 = _origin.x;
    auto const y1 = _origin.y;

    auto const x2 = static_cast<uint32_t>(std::round(_sides.first * cos) + x1);
    auto const y2 = static_cast<uint32_t>(std::round(_sides.first * sin) + y1);

    auto const x3 = static_cast<uint32_t>(std::round(_sides.first * cos - _sides.second * sin) + x1);
    auto const y3 = static_cast<uint32_t>(std::round(_sides.first * sin + _sides.second * cos) + y1);

    auto const x4 = static_cast<uint32_t>(- std::round(_sides.second * sin) + x1);
    auto const y4 = static_cast<uint32_t>(std::round(_sides.second * cos) + y1);

    if (_fill_color.a != 0) {
        using segment = std::pair<vertex, vertex>;
        auto segments = std::priority_queue<segment, std::vector<segment>, decltype(detail::segment_compare)>{};
        segments.push({{x1, y1}, {x2, y2}});
        segments.push({{x2, y2}, {x3, y3}});
        segments.push({{x3, y3}, {x4, y4}});
        segments.push({{x1, y1}, {x4, y4}});

        auto y = detail::top_vertex({segments.top().first, segments.top().second}).y - 1;
        fmt::print(stderr, "Top vertex y: {}\n", y);
        img.draw(line{{0, y}, {299, y}, color::blue});
        fmt::print(stderr, "Drawn\n");

        auto pop = [&segments]() mutable 
            -> std::optional<segment>
        {
            if (segments.empty()) {
                return std::nullopt;
            }
            auto [p, q] = segments.top();
            segments.pop();
            auto [p2, q2] = std::ranges::minmax(p, q, std::less{}, &vertex::y);
            return segment{q2, p2};
        };

        auto m_and_q = [](segment const s) noexcept {
            auto const m = (s.first.y - s.second.y) * 1.f / (s.first.x - s.second.x);
            auto const q = s.first.y - m * s.first.x;
            return std::pair{m, q};
        };

        auto a = pop();
        auto [a_m, a_q] = m_and_q(*a);
        auto b = pop();
        auto [b_m, b_q] = m_and_q(*b);

        while (true) {
            if (y < a->second.y) {
                a = pop();
                if (not a.has_value()) {
                    break;
                }
                std::tie(a_m, a_q) = m_and_q(*a);
            }
            if (y < b->second.y) {
                b = pop();
                if (not b.has_value()) {
                    break;
                }
                std::tie(b_m, b_q) = m_and_q(*b);
            }

            // trovare le x corrispondente alla y corrente
            auto x_1 = static_cast<uint32_t>(std::round((y - a_q) * 1.f / a_m));
            auto x_2 = static_cast<uint32_t>(std::round((y - b_q) * 1.f / b_m));

            img.draw(line{{x_1, y}, {x_2, y}, _fill_color, false});
            --y;
        }
    }


    img.draw(line{{x1, y1}, {x2, y2}, _border_color, _anti_aliasing});
    img.draw(line{{x2, y2}, {x3, y3}, _border_color, _anti_aliasing});
    img.draw(line{{x3, y3}, {x4, y4}, _border_color, _anti_aliasing});
    img.draw(line{{x4, y4}, {x1, y1}, _border_color, _anti_aliasing});
}
} // namespace spl::graphics


