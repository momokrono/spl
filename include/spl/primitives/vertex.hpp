/**
 * @author      : rbrugo, momokrono
 * @file        : vertex
 * @created     : Friday Mar 05, 2021 12:30:12 CET
 * @license     : MIT
 * */

#ifndef PRIMITIVES_VERTEX_HPP
#define PRIMITIVES_VERTEX_HPP

#include <cstdint>

namespace spl::graphics
{

struct vertex
{
    int_fast32_t x;
    int_fast32_t y;

    friend constexpr vertex & operator+=(vertex & self, vertex v) noexcept {
        self.x += v.x;
        self.y += v.y;
        return self;
    }

    friend constexpr vertex operator-(vertex & self) noexcept {
        return vertex{-self.x, -self.y};
    }

    friend constexpr vertex & operator-=(vertex & self, vertex v) noexcept {
        return self += -v;
    }

    friend constexpr vertex operator+(vertex self, vertex v) noexcept {
        return v += self;
    }

    friend constexpr vertex operator-(vertex self, vertex v) noexcept {
        return v -= self;
    }

    friend constexpr vertex operator*=(vertex & self, float a) noexcept {
        self.x *= a;
        self.y *= a;
        return self;
    }

    friend constexpr vertex operator*(vertex self, float a) noexcept {
        return self *= a;
    }

    friend constexpr vertex operator*(float a, vertex self) noexcept {
        return self *= a;
    }

    friend constexpr vertex operator/=(vertex & self, float a) noexcept {
        self.x /= a;
        self.y /= a;
        return self;
    }

    friend constexpr vertex operator/(vertex self, float a) noexcept {
        return self /= a;
    }
};

} // namespace spl::graphics

#endif /* PRIMITIVES_VERTEX_HPP */

