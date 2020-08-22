/**
 * @author      : rbrugo, momokrono
 * @file        : primitive
 * @created     : Monday Jul 13, 2020 02:31:15 CEST
 * @license     : MIT
 */

#include "spl/graphics/primitive.hpp"
#include <numbers>

namespace spl::graphics
{

namespace detail
{
    template <typename SegmentList = std::vector<std::pair<vertex, vertex>>>
    void draw_filled(image & img, SegmentList && segments_list, spl::graphics::rgba const fill_color)
    {
        using segment = std::pair<vertex, vertex>;
        auto segments = std::priority_queue{detail::segment_compare, std::forward<SegmentList>(segments_list)};

        auto y = detail::top_vertex({segments.top().first, segments.top().second}).y - 1;

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
            auto x_1 = std::isinf(a_m)
                     ? a->first.x
                     : static_cast<int_fast32_t>(std::round((y - a_q) * 1.f / a_m));
            auto x_2 = std::isinf(b_m)
                     ? b->first.x
                     : static_cast<int_fast32_t>(std::round((y - b_q) * 1.f / b_m));

            img.draw(line{{x_1, y}, {x_2, y}, fill_color, false});
            --y;
        }
    }
} // namespace detail

void line::render_on(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    auto const width  = img.swidth();
    auto const height = img.sheight();

    if (x1 == x2) {
        if (x1 < 0 or x1 >= width) {
            return;
        }
        auto [from, to] = std::minmax({y1, y2});
        from = std::max(from, 0l);
        to   = std::min(to, height - 1);

        for (; from <= to; ++from) {
            auto & pixel = img.pixel(x1, from);
            pixel = over(color, pixel);
        }
    } else if (y1 == y2) {
        if (y1 < 0 or y1 >= height) {
            return;
        }
        auto [from, to] = std::minmax({x1, x2});
        if (from > width - 1 or to < 0) {
            return;
        }
        from = std::max(from, 0l);
        to   = std::min(to, width - 1);

        for (; from <= to; ++from) {
            auto & pixel = img.pixel(from, y1);
            pixel = over(color, pixel);
        }
    } else if (anti_aliasing) {
        draw_antialiased_parametric(img);
    }
    else {
        draw_aliased(img);
    }
}

void line::draw_antialiased_parametric(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    auto const width  = img.swidth();
    auto const height = img.sheight();

    if (x2 < x1) {
        std::swap(x2, x1);
        std::swap(y2, y1);
    }
    auto const dx = std::abs(x2 - x1);
    auto const dy = std::abs(y2 - y1);
    auto const distance = dx + dy; // discrete plane => manhattan distance

    auto const kx = dx * 1.f / distance;
    auto const ky = dy * 1.f / distance * ((y2 > y1) * 2 - 1);

    int_fast32_t t = 0;
    for (; t < distance; ++t) {
        auto const x = x1 + t * kx;
        auto const y = y1 + t * ky;

        if ((std::floor(x) >= 0 and std::floor(y) >= 0) and (std::ceil(x) < width and std::ceil(y) < height)) {
            break;
        }
    }

    for (; t < distance; ++t) {
        auto const x = x1 + t * kx;
        auto const y = y1 + t * ky;

        for (auto const rounded_y : { std::floor(y), std::ceil(y) }) {
            auto const y_blend = 1.f - std::abs(y - rounded_y);
            for (auto const rounded_x : { std::floor(x), std::ceil(x) }) {
                if (rounded_x >= width or rounded_y >= height) {
                    return;
                }
                if (rounded_x < 0 or rounded_y < 0) {
                    return;
                }
                auto const x_blend = 1.f - std::abs(x - rounded_x);

                auto & pixel = img.pixel(rounded_x, rounded_y);
                pixel = over(color.blend(y_blend * x_blend), pixel);
            }
        }
    }
    // one last time for the final pixel
    {
        auto const x = x1 + t * kx;
        auto const y = y1 + t * ky;

        auto const floorx = std::floor(x);
        auto const floory = std::floor(y);

        if (not (x >= width or y >= height or floorx < 0 or floory < 0)) {
            auto const y_blend = 1.f - std::abs(y - floory);
            auto const x_blend = 1.f - std::abs(x - floorx);

            auto & pixel = img.pixel(floorx, floory);
            pixel = over(color.blend(y_blend * x_blend), pixel);
        }
    }
}

void line::draw_aliased(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    auto const width  = img.swidth();
    auto const height = img.sheight();

    if (std::abs(x2 - x1) >= std::abs(y2 - y1)) {
        auto const m = (y2 - y1) * 1.f / (x2 - x1);
        auto const q = y2 - m * x2;

        auto [from, to] = std::minmax({x1, x2});
        from = std::clamp(from, 0l, width - 1);
        to   = std::clamp(to,   0l, width - 1);
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            auto const y = m * from + q;
            if (y >= 0 and y < height) {
                img.pixel(from, std::lround(y)) = color;
            }
        }
    } else {
        auto const m_rev = (x2 - x1) * 1.f / (y2 - y1);
        auto const q = y2 - x2 / m_rev;

        auto [from, to] = std::minmax({y1, y2});
        from = std::clamp(from, 0l, height - 1);
        to   = std::clamp(to,   0l, height - 1);
        while (from < 0) { ++from; }
        for (; from <= to; ++from) {
            // y = mx + q
            // x = (y - q)/m
            auto const x = (from - q) * m_rev;
            if (x >= 0 and x < width) {
                img.pixel(std::lround(x), from) = color;
            }
        }
    }
}

void line::draw_antialiased(image & img)
{
    auto [x1, y1] = start;
    auto [x2, y2] = end;

    if (std::abs(x2 - x1) >= std::abs(y2 - y1)) {
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
    static_assert(std::is_signed_v<decltype(x1)>);

    auto const x2 = static_cast<int_fast32_t>(std::round(_sides.first * cos) + x1);
    auto const y2 = static_cast<int_fast32_t>(std::round(_sides.first * sin) + y1);

    auto const x3 = static_cast<int_fast32_t>(std::round(_sides.first * cos - _sides.second * sin) + x1);
    auto const y3 = static_cast<int_fast32_t>(std::round(_sides.first * sin + _sides.second * cos) + y1);

    auto const x4 = static_cast<int_fast32_t>(- std::round(_sides.second * sin) + x1);
    auto const y4 = static_cast<int_fast32_t>(std::round(_sides.second * cos) + y1);

    if (_fill_color.a != 0) {
        detail::draw_filled(img, {{{x1, y1}, {x2, y2}}, {{x2, y2}, {x3, y3}}, {{x3, y3}, {x4, y4}}, {{x1, y1}, {x4, y4}}}, _fill_color);
    }


    img.draw(line{{x1, y1}, {x2, y2}, _border_color, _anti_aliasing});
    img.draw(line{{x2, y2}, {x3, y3}, _border_color, _anti_aliasing});
    img.draw(line{{x3, y3}, {x4, y4}, _border_color, _anti_aliasing});
    img.draw(line{{x4, y4}, {x1, y1}, _border_color, _anti_aliasing});
}

void regular_polygon::_draw_unfilled(image & img)
{
    auto const [x_c, y_c] = _center;
    auto const theta = 2 * std::numbers::pi / _sides;
    auto const theta_0 = theta / 2 + _rotation;
    auto const len   = _radius * std::sqrt(2 - std::cos(theta));
    auto       x_p   = x_c - len * std::sin(theta_0);
    auto       y_p   = y_c - len * std::cos(theta_0);

    for (auto t = 0.; t < 2 * std::numbers::pi; t += theta) {
        auto const new_x_p = x_p + len * std::sin(theta_0 + t);
        auto const new_y_p = y_p + len * std::cos(theta_0 + t);

        img.draw(spl::graphics::line{
            {static_cast<int_fast32_t>(x_p),     static_cast<int_fast32_t>(y_p)},
            {static_cast<int_fast32_t>(new_x_p), static_cast<int_fast32_t>(new_y_p)},
            _border_color,
            _anti_aliasing
        });
        x_p = new_x_p;
        y_p = new_y_p;
    }
}

void regular_polygon::_draw_filled(image & img)
{
    auto const [x_c, y_c] = _center;
    auto const theta = 2 * std::numbers::pi / _sides;
    auto const theta_0 = theta / 2 + _rotation;
    auto const len   = _radius * std::sqrt(2 - std::cos(theta));
    auto       x_p   = x_c - len * std::sin(theta_0);
    auto       y_p   = y_c - len * std::cos(theta_0);

    auto segments = std::vector<std::pair<vertex, vertex>>{};
    segments.reserve(_sides);

    for (auto t = 0.; t < 2 * std::numbers::pi; t += theta) {
        auto const new_x_p = x_p + len * std::sin(theta_0 + t);
        auto const new_y_p = y_p + len * std::cos(theta_0 + t);

        segments.emplace_back(
            vertex{static_cast<int_fast32_t>(x_p),     static_cast<int_fast32_t>(y_p)},
            vertex{static_cast<int_fast32_t>(new_x_p), static_cast<int_fast32_t>(new_y_p)}
        );
        x_p = new_x_p;
        y_p = new_y_p;
    }

    detail::draw_filled(img, segments, _fill_color);

    for (auto const [v1, v2] : segments) {
        img.draw(spl::graphics::line{v1, v2, _border_color, _anti_aliasing });
    }
}

} // namespace spl::graphics


