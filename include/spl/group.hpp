/**
 * @author      : rbrugo, momokrono
 * @file        : renderer
 * @created     : Tuesday Nov 10, 2020 10:54:18 CET
 * @license     : MIT
 * */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "spl/any_drawable.hpp"
#include "spl/viewport.hpp"
#include "spl/primitives/vertex.hpp"

namespace spl::graphics
{

class group
{
    std::vector<any_drawable> _buffer;
    vertex _origin{0, 0};

public:
    void render_on(graphics::viewport img) const noexcept;
    template <drawable T>
    group & push(T obj) noexcept;
    void clear() noexcept { _buffer.clear(); }
    void reserve(std::size_t n) { _buffer.reserve(n); }
    auto position()       noexcept -> vertex & { return _origin; }
    auto position() const noexcept -> vertex   { return _origin; }
    auto & translate(int_fast32_t x, int_fast32_t y) { _origin += {x, y}; return *this; }
};

inline
void group::render_on(graphics::viewport img) const noexcept
{
    auto view = viewport{img, _origin.x, _origin.y};
    for (auto const & obj : _buffer) {
        obj.render_on(view);
    }
}

template <drawable T>
inline
auto group::push(T obj) noexcept -> group &
{
    _buffer.emplace_back(std::move(obj));
    return *this;
}

} // namespace spl::graphics

#endif /* RENDERER_HPP */

