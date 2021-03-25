/**
 * @author      : rbrugo, momokrono
 * @file        : primitive
 * @created     : Monday Jul 13, 2020 02:31:15 CEST
 * @license     : MIT
 */

#include "spl/primitive.hpp"
#include <numbers>

namespace spl::graphics
{

namespace detail
{
    template <typename SegmentList = std::vector<std::pair<vertex, vertex>>>
    void draw_filled(image & img, SegmentList && segments_list, spl::graphics::rgba const fill_color) noexcept
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

void line::render_on(image & img) const noexcept
{
    if (thickness <= 0) {
        return;
    }
    if (thickness > 1) {
        _draw_thick(img);
        return;
    }
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

void line::draw_antialiased_parametric(image & img) const noexcept
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

void line::draw_aliased(image & img) const noexcept
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

void line::draw_antialiased(image & img) const noexcept
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


void line::_draw_thick(image & img) const noexcept
{
    // assumes thicknes > 1
    if (not anti_aliasing) {
        auto const [x1, y1] = start;
        auto const [x2, y2] = end;
        auto const length = std::hypot(x2 - x1, y2 - y1);
        auto const inclination = std::atan2(y2 - y1, x2 - x1);

        auto const sin = std::sin(inclination);
        auto const cos = std::cos(inclination);

        auto const rect = rectangle(
            {int_fast32_t(x1 + thickness * sin / 2), int_fast32_t(y1 - thickness * cos / 2)},
            {length, thickness - 1}, inclination, false
        ).fill_color(color).border_color(color);

        rect.render_on(img);
        return;
    }

    auto [x1, y1] = start;
    auto [x2, y2] = end;
    auto const radius = thickness / 2.f;
    auto const width = img.swidth();
    auto const height = img.sheight();

    auto const m = (y2 - y1) * 1. / (x2 - x1);
    auto const q = y2 - m * x2;

    // auto [from, to] = std::minmax({x1, x2});
    auto [pt_from, pt_to] = std::ranges::minmax({start, end}, std::ranges::less{}, &spl::graphics::vertex::x);
    auto [from, from_y] = pt_from;
    auto [to, to_y] = pt_to;

    auto const dy = m >= 0 ? 1 : -1;
    auto const x_off = radius;
    auto const y_off = radius * dy;

    auto const min_x = std::max<float>(0, from - x_off);
    auto const max_x = std::min<float>(to + x_off, width - 1);

    auto const distance = [=, inv_den = 1 / std::sqrt(1 + m * m)](auto const x, auto const y) {
        // Check if (x,y) in the plane delimited by the lines passing for from and to, and orthogonal
        //  to the segment from-to
        auto const q1 = m != 0 ? from_y + from / m : from;
        auto const q2 = m != 0 ? to_y   + to   / m : to;
        auto const q3 = m != 0 ? y      + x    / m : x;

        // If the point is not in the plane, calculate the distance relative to the nearest point
        if (q3 < std::min(q1, q2)) {
            auto varx = q1 <= q2 ? from : to;
            auto vary = q1 <= q2 ? from_y : to_y;
            return std::hypot<float>(x - varx, y - vary);
        }

        if (q3 > std::max(q1, q2)) {
            auto varx = (q1 >= q2) ? from : to;
            auto vary = (q1 >= q2) ? from_y : to_y;
            return std::hypot<float>(x - varx, y - vary);
        }

        // If it is in the plane, calculate the line-point distance
        return std::abs(y - m * x - q) * inv_den;
    };

    // Weight normalized on the distance
    auto const weight = [&distance, norm = 1.f / radius](auto const x, auto const y) {
        return 1 - distance(x, y) * norm;
    };

    auto y = std::clamp<float>(
        m * (from - radius * std::numbers::sqrt2 / 2) + q - y_off,
        0,
        width - 1
    );

    for (; std::abs(m * to + y_off - y) > 0.01 and 0 <= y and y < width; y += dy) {
        for (int_fast32_t const effective_y : {std::floor(y), std::ceil(y)}) {
            if (effective_y < 0 or effective_y >= height) {
                continue;
            }
            auto x = min_x;
            for (auto [d, prev_d] = std::pair{distance(x, effective_y), width * 1.f};
                 d > radius and x <= max_x;)  {
                ++x;
                prev_d = (std::exchange(d, distance(x, effective_y)));
                if (prev_d < d) {
                    x = max_x + 1;
                }
            }
            if (x > max_x) {
                continue;
            }

            for (; distance(x, effective_y) <= radius and x < max_x; ++x) {
                for (int_fast32_t const effective_x : {std::floor(x), std::ceil(x)}) {
                    if (effective_x >= width) {
                        break;
                    }
                    auto const w = weight(effective_x, effective_y);
                    if (w > 0) {
                        auto & pixel = img.pixel(effective_x, effective_y);
                        pixel = over(color.blend(w), pixel);
                    }

                }
            }
        }
    }
}

#ifdef PRIMITIVES_BEZIER_HPP
template <typename Alloc>
void detail::_bezier_render_aliased(image & img, std::span<vertex> const v, spl::graphics::rgba const color)
{
    switch (std::ssize(v)) {
    case 2: {
        // linear
        /*
        auto const [x0, y0] = v.front();
        auto const [x1, y1] = v.back();

        auto const dx = std::abs(x1 - x0);
        auto const dy = std::abs(y1 - y0);
        auto const distance = std::max(dx, dy);
        auto const distance_inv = 1.f / distance;

        for (int_fast32_t i = 0;V i < distance; ++i) {
            auto const t = i * distance_inv;
            auto const x = (1 - t) * x0 + t * x1;
            auto const y = (1 - t) * y0 + t * y1;

            img.pixel(x, std::lround(y)) = color;
        }*/
        img.draw(line{v.front(), v.back(), color, false});
        break;
    }
    case 3: {
        // quadratic
        auto const [x0, y0] = v.front();
        auto const [x1, y1] = v[1];
        auto const [x2, y2] = v.back();

        auto const dx = std::max({x0, x1, x2}) - std::min({x0, x1, x2});
        auto const dy = std::max({y0, y1, y2}) - std::min({y0, y1, y2});
        auto const distance = std::max(dx, dy);
        auto const distance_inv = 1.f / distance;

        for (int_fast32_t i = 0; i < distance; ++i) {
            auto const t = i * distance_inv;
            auto const x = (1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2;
            auto const y = (1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2;

            img.pixel(x, std::lround(y)) = color;
        }

        // B(t) = (1-t)²P0 + 2t(1-t)P1 + t²P2
        break;
    }
    case 4:
        // cubic
        auto const [x0, y0] = v.front();
        auto const [x1, y1] = v[1];
        auto const [x2, y2] = v[2];
        auto const [x3, y3] = v.back();

        auto const dx = std::max({x0, x1, x2, x3}) - std::min({x0, x1, x2, x3});
        auto const dy = std::max({y0, y1, y2, y3}) - std::min({y0, y1, y2, y3});
        auto const distance = std::max(dx, dy);
        auto const distance_inv = 1.f / distance;

        for (int_fast32_t i = 0; i < distance; ++i) {
            auto const t = i * distance_inv;
            auto const u = 1 - t;
            auto const x =     u * u * u * x0
                         + 3 * u * u * t * x1
                         + 3 * u * t * t * x2
                         +     t * t * t * x3;
            auto const y =     u * u * u * y0
                         + 3 * u * u * t * y1
                         + 3 * u * t * t * y2
                         +     t * t * t * y3;

            img.pixel(x, std::lround(y)) = color;
        }

        // B(t) = (1-t)³P0 + 3t(1-t)²P1 + 3t²(1 - t)P2 + t³P3
        break;
    default: {
        auto const [x_min, x_max] = std::ranges::minmax_element(v, std::ranges::less{}, &vertex::x);
        auto const [y_min, y_max] = std::ranges::minmax_element(v, std::ranges::less{}, &vertex::y);

        auto const binomial_coefficient = [](auto n, auto k) -> std::uint64_t {
            if (k == 0 or k == n) {
                return 1;
            }
            if (k > n) {
                return 0;
            }
            if (k > n - k) {
                k = n - k;
            }
            if (k == 1) {
                return n;
            }

            auto b = 1.; //std::uint64_t{1};
            for (int_fast32_t i = 1; i <= k; ++i) {
                b *= (n - (k - i));
                b /= i;
            }

            return b;
        };
    }
    }

}
#endif // PRIMITIVES_BEZIER_HPP

void rectangle::render_on(image & img) const noexcept
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
        detail::draw_filled(img, {
            {{x1, y1}, {x2, y2}},
            {{x2, y2}, {x3, y3}},
            {{x3, y3}, {x4, y4}},
            {{x1, y1}, {x4, y4}}}, _fill_color);
    }


    img.draw(line{{x1, y1}, {x2, y2}, _border_color, _anti_aliasing});
    img.draw(line{{x2, y2}, {x3, y3}, _border_color, _anti_aliasing});
    img.draw(line{{x3, y3}, {x4, y4}, _border_color, _anti_aliasing});
    img.draw(line{{x4, y4}, {x1, y1}, _border_color, _anti_aliasing});
}

void regular_polygon::_draw_unfilled(image & img) const noexcept
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

void regular_polygon::_draw_filled(image & img) const noexcept
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

void circle::_draw_unfilled(image & img) const noexcept
{
    // Bresenham circle algorithm

    // The circle is divided in 8 symmetric parts
    // The starting point is (x, y)
    // The next point is P':=(x+1, y) or P'':=(x+1, y-1), I have to decide between them
    // We can define a "radius error" function as follow:
    // e(P) = distance(P)² - r² = (x_p)² + (y_p)² - r²
    // With this new function it is possible to define a new "decision parameter":
    //
    // d := e(P') + e(P'')
    //    = (x+1)² + y² - r² + (x+1)² + (y+1)² - r²
    //    = 2x² + 3 + 4x + 2y² - 2y - 2r²
    // If I assume that the first point is (0, r),
    //    = 0   + 3 +  0 + 2r² - 2r - 2r²
    //    = 3 - 2r
    //
    // If d < 0, P' is a better fit than P''. If d >= 0, is the other way.
    //
    // After the first calculation, it can be proved that d must be updated using the following rule:
    //    · d = d + 2*x + 1          if d < 0
    //    · d = d + 2*(x - y) + 1    if d >= 0

    auto const r = _radius;
    auto d = 3 - 2 * r;

    auto x = int_fast32_t{0};
    auto y = int_fast32_t(r);
    while (x <= y) {
        _draw_sym_points(img, x, y);
        _draw_sym_points(img, y, x);
        ++x;
        if (d >= 0) {
            --y;
            d += 2 * (x - y) + 1;
        } else {
            d += 2 * x + 1;
        }
    }
}

void circle::_draw_filled(image & img) const noexcept
{
    auto const r = _radius;
    auto d = 3 - 2 * r;

    auto x = int_fast32_t{0};
    auto y = int_fast32_t(r);
    while (x <= y) {
        auto const old_x = x;
        auto const old_y = y;
        ++x;
        _draw_fill_lines(img, y, x - 1);
        if (d >= 0) {
            --y;
            _draw_fill_lines(img, x - 1, y);
            d += 2 * (x - y) + 1;
        } else {
            d += 2 * x + 1;
        }

        _draw_sym_points(img, old_x, old_y);
        _draw_sym_points(img, old_y, old_x);
    }
}

} // namespace spl::graphics


