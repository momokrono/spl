/**
 * @author      : momokrono, rbrugo
 * @file        : primitive
 * @created     : Saturday Jun 06, 2020 23:20:38 CEST
 * @license     : MIT
 * */

#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <cmath>

#include "rgba.hpp"
#include "image.hpp"

namespace spl::graphics
{

struct vertex
{
    int_fast32_t x;
    int_fast32_t y;
};

class primitive
{
public:
};

class circle : public primitive
{
    bool filled = false;
    rgba color;
    uint_fast32_t radius;
    vertex center;
public:
};

struct line // : public primitive
{
    vertex start;
    vertex end;
    rgba color;
    bool anti_aliasing = true;
    // uint8_t thickness;

    auto render_on(image & img)
    {
        if (anti_aliasing) {
            draw_antialiased(img);
        }
        else {
            draw_aliased(img);
        }
    }

    void draw_antialiased_parametric(image & img)
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

    void draw_aliased(image & img)
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

    void draw_antialiased(image & img)
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
};

class rectangle : public primitive
{
public:
};

// template <uint8_t N>
struct regular_n_agon // : public primitive
{
    vertex  center;
    uint8_t sides;
    float rotation;
    bool filled = false;

    /* auto render_on(image & img) */
    /* { */
    /*     if (filled) { */
    /*         /1* _draw_filled(img); *1/ */
    /*     } else { */
    /*         /1* _draw_unfilled(img); *1/ */
    /*     } */
    /* } */

private:
    /* auto _draw_filled */
};

} // namespace spl::graphics

#endif /* PRIMITIVE_HPP */

