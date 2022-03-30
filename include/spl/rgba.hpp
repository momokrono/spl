#ifndef RGBA_HPP
#define RGBA_HPP

#include <cstdint>
#include <concepts>

#include "spl/detail/exceptions.hpp"

namespace spl::graphics
{

/**
 * An object representing a color
 * */
struct rgba
{
    /** Construct a `rgba` without specifying the underlying color */
    constexpr rgba() = default;
    /** Construct a `rgba` specifying the underlying color channels */
    constexpr rgba(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) : r{_r}, g{_g}, b{_b}, a{_a} {}

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    // std::same_as<uint8_t>
    /** \rst
     Applies a new transparency in range :math:`[0, 255]` to the color
     \endrst
     */
    [[nodiscard]] constexpr inline
    auto blend(uint8_t a2) const noexcept
    {
        a2 = static_cast<uint8_t>((a2 / 255.f) * a);
        return rgba{r, g, b, a2};
    }

    /** \rst
     Applies a new transparency in range :math:`[0.0, 1.0]` to the color
     \endrst
     */
    constexpr inline
    auto blend(std::floating_point auto a2) const
    {
        if (a2 < 0. or a2 > 1.) {
            throw spl::invalid_argument{"'rgba::value(std::floating_point auto)' expects a value"
                                        " between 0. and 1."};
        }

        a2 *= a;
        return rgba{r, g, b, static_cast<uint8_t>(a2)};
    }

    /** Converts the current color in an equivalent rgb color with full opacity */
    [[nodiscard]] constexpr inline
    auto to_rgb() const noexcept
    {
        auto const alpha = a / 255.f;
        return rgba{
            static_cast<uint8_t>(r * alpha),
            static_cast<uint8_t>(g * alpha),
            static_cast<uint8_t>(b * alpha),
            255
        };
    }

    /** Compare two `rgba`, channel by channel */
    friend constexpr
    bool operator==(rgba, rgba) = default;
};

/**
 * Blends `background` upon `foreground` without applying gamma correction
 *
 * @param foreground the color that will be applied upon the `background` color
 * @param background the color that will be used as base for the `over` operation
 * @return the color resulting from the application of `foreground` on top of `background`
 *         neglecting the gamma correction
 * */
constexpr
auto over_no_gamma(rgba foreground, rgba background)
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

/**
 * Convert a color in grayscale
 *
 * @param p the color to be converted
 * @return the resulting grayscaled `rgba`
 * */
constexpr inline
auto grayscale(rgba p)
{
    constexpr auto red_coeff = 0.299;
    constexpr auto green_coeff = 0.587;
    constexpr auto blue_coeff = 0.114;
    auto rgb = static_cast<uint8_t>(p.r * red_coeff + p.g * green_coeff + p.b * blue_coeff);
    return rgba{rgb, rgb, rgb, p.a};
}

/**
 * Perform an "over composition" of `foreground` over `background`.
 * By default, gamma correction will be performed in the operation; you can disable
 * it by defining `#SPL_DISABLE_GAMMA_CORRECTION` (in that case, this functions is
 * just an alias for `over_no_gamma`)
 *
 * @param foreground the color that will be applied upon the `background` color
 * @param background the color that will be used as base for the `over` operation
 * @return the color resulting from the application of `foreground` on top of `background`
 * */
#ifdef SPL_DISABLE_GAMMA_CORRECTION
constexpr
auto over(rgba foreground, rgba background)
{
    return over_no_gamma(foreground, background);
}
#else
constexpr
auto over(rgba foreground, rgba background)
    -> rgba
{
    auto const [r1, g1, b1, a1] = foreground;
    auto const [r2, g2, b2, a2] = background;

    auto const k1 = a1 / 255.f;
    auto const k2 = (1.f - k1) * a2 / 255.f;

    auto const over_impl = [k1, k2](uint8_t c1, uint8_t c2) noexcept {
        return static_cast<uint8_t>(std::sqrt((c1 * c1 * k1 + c2 * c2 * k2) / (k1 + k2)));
    };

    return {
        over_impl(r1, r2),
        over_impl(g1, g2),
        over_impl(b1, b2),
        static_cast<uint8_t>((k1 + k2) * 255)
    };
}
#endif

/** A namespace containing a predefined set of `rgba` colors */
namespace color
{
    constexpr inline rgba red     {255,   0,      0,      255};
    constexpr inline rgba green   {0,     255,    0,      255};
    constexpr inline rgba blue    {0,     0,      255,    255};
    constexpr inline rgba white   {255,   255,    255,    255};
    constexpr inline rgba black   {0,     0,      0,      255};
    constexpr inline rgba magenta {255,   0,      255,    255};
    constexpr inline rgba cyan    {0,     255,    255,    255};
    constexpr inline rgba yellow  {255,   255,    0,      255};
    constexpr inline rgba gray    {128,   128,    128,    255};
    constexpr inline rgba violet  {238,   130,    238,    255};
    constexpr inline rgba orange  {255,   165,    0,      255};
    constexpr inline rgba brown   {165,   42,     42,     255};

    constexpr inline rgba nothing {0,     0,      0,      0};
} // namespace color
} // namespace spl::graphics

#endif /* RGBA_HPP */
