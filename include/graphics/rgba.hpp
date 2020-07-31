#ifndef _RGBA_HPP_
#define _RGBA_HPP_

#include <cstdint>
#include <concepts>

#include "bits/exceptions.hpp"

namespace spl::graphics
{
struct rgba
{
    constexpr rgba() = default;
    constexpr rgba(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) : r{_r}, g{_g}, b{_b}, a{_a} {}
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    // std::same_as<uint8_t>
    constexpr inline
    auto blend(uint8_t a2) noexcept
    {
        a2 = static_cast<uint8_t>((a2 / 255.f) * a);
        return rgba{r, g, b, a2};
    }

    constexpr inline
    auto blend(std::floating_point auto a2)
    {
        if (a2 < 0. or a2 > 1.) {
            throw spl::invalid_argument{"'rgba::value(std::floating_point auto)' expects a value"
                                        " between 0. and 1."};
        }

        a2 *= a;
        return rgba{r, g, b, static_cast<uint8_t>(a2)};
    }

    constexpr inline
    auto to_rgb() noexcept
    {
        auto const alpha = a / 255.f;
        return rgba{
            static_cast<uint8_t>(r * alpha),
            static_cast<uint8_t>(g * alpha),
            static_cast<uint8_t>(b * alpha),
            255};
    }

    constexpr
    friend bool operator==(rgba const, rgba const) = default;
};

constexpr
auto over(rgba const foreground, rgba const background)
    -> rgba
{
    auto const [r1, g1, b1, a1] = foreground;
    auto const [r2, g2, b2, a2] = background;

    auto const k1 = a1 / 255.f;
    auto const k2 = (1.f - k1) * a2 / 255.f;

    auto const over_impl = [k1, k2](uint8_t c1, uint8_t c2) noexcept {
        return static_cast<uint8_t>((c1 * k1 + c2 * k2) / (k1 + k2));
    };

    return {
        over_impl(r1, r2),
        over_impl(g1, g2),
        over_impl(b1, b2),
        static_cast<uint8_t>((k1 + k2) * 255)
    };
}

namespace color
{
    constexpr const rgba red     {255,   0,      0,      255};
    constexpr const rgba green   {0,     255,    0,      255};
    constexpr const rgba blue    {0,     0,      255,    255};
    constexpr const rgba white   {255,   255,    255,    255};
    constexpr const rgba black   {0,     0,      0,      255};
    constexpr const rgba magenta {255,   0,      255,    255};
    constexpr const rgba cyan    {0,     255,    255,    255};
    constexpr const rgba yellow  {255,   255,    0,      255};
    constexpr const rgba gray    {128,   128,    128,    255};
    constexpr const rgba violet  {238,   130,    238,    255};
    constexpr const rgba orange  {255,   165,    0,      255};
    constexpr const rgba brown   {165,   42,     42,     255};

    constexpr const rgba nothing {0,     0,      0,      0};
} // namespace color
} //namespace spl::graphics

#endif
