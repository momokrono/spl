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

/**
  A `vertex` is a pair of integers representing a point on an `image`

  The `vertex` class supports a bunch of mathematical operations such as
  - `operator+=` and `operator+` to sum two vertexes (as in a point-vector sum)
  - `operator-=` and `operator-` to subtract two vertexes (as in a point-point difference)
  - unary `operator-`
  - `operator*=`, `operator/=`, `operator*`, `operator/` for multiplication and division by a scalar
 * */
struct vertex
{
    using value_type = int_fast32_t;

    value_type x;
    value_type y;

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
        return self += v;
    }

    friend constexpr vertex operator-(vertex self, vertex v) noexcept {
        return self -= v;
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

