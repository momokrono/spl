#ifndef _RGBA_HPP_
#define _RGBA_HPP_

#include <cstdint>

namespace spl::graphics
{
struct rgba
{
    constexpr rgba() = default;
    constexpr rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r{r}, g{g}, b{b}, a{a} {}
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    constexpr
    friend bool operator==(rgba const, rgba const) = default;
};

namespace color
{
    constexpr const rgba red     {255,   0,      0,      255};
    constexpr const rgba green   {0,     255,    0,      255};
    constexpr const rgba blue    {0,     0,      255,    255};
    constexpr const rgba white   {255,   255,    255,    255};
    constexpr const rgba black   {0,     0,      0,      255};
    constexpr const rgba nothing {0,     0,      0,      0};
} // namespace color
} //namespace spl::graphics

#endif
