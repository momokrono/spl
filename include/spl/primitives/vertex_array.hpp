#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include <vector>
#include "spl/primitives/vertex.hpp"
#include "spl/rgba.hpp"
#include "spl/image.hpp"

namespace spl::graphics
{

class vertex_array{

public:
    
    enum class types { points, lines, strips, triangles, fan, triangles_strips };

    // constexpr ?
    explicit vertex_array(spl::graphics::rgba c = spl::graphics::color::black, types t = types::points): _color{c}, _type{t} {}

    void render_on(spl::graphics::image & img) const noexcept;
    vertex_array & push(vertex v) noexcept;
    void clear() noexcept { _buffer.clear(); }
    void reserve(std::size_t n) { _buffer.reserve(n); }

    rgba & color() { return _color; }
    rgba color() const { return _color; }

    types & type() { return _type; }
    types type() const { return _type; }

private:
    
    std::vector<spl::graphics::vertex> _buffer;
    spl::graphics::rgba _color;
    types _type;
};

inline
auto vertex_array::push(spl::graphics::vertex v) noexcept -> vertex_array &
{
    _buffer.emplace_back(v);
    return *this;
}

inline
void vertex_array::render_on(graphics::image & img) const noexcept
{
	// TODO
}

} // namespace spl::graphics

#endif