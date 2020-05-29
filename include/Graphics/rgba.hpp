#ifndef _RGBA_HPP_
#define _RGBA_HPP_

#include <cstdint>

namespace spl::graphics
{
struct rgba
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    friend bool operator==(rgba const, rgba const) = default;
};

} //namespace spl::graphics

#endif
