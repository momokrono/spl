#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include <vector>
#include <optional>
#include "spl/primitives/vertex.hpp"
#include "spl/primitives/line.hpp"
#include "spl/rgba.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

class vertex_array
{
public:
    enum class types { points, lines, strips, triangles, fan, triangles_strips };

    // constexpr ?
    explicit
    vertex_array(spl::graphics::rgba c = spl::graphics::color::black, types t = types::points) :
        _color{c}, _type{t}
    {}

    void render_on(spl::graphics::viewport img) const noexcept;
    vertex_array & push(vertex v, std::optional<spl::graphics::rgba> c=std::nullopt) noexcept;
    void clear() noexcept { _buffer.clear(); }
    void reserve(std::size_t n) { _buffer.reserve(n); }

    rgba & color() { return _color; }
    rgba color() const { return _color; }

    types & type() { return _type; }
    types type() const { return _type; }

private:
    std::vector<std::tuple<int_fast32_t, int_fast32_t, std::optional<spl::graphics::rgba>>> _buffer;
    spl::graphics::rgba _color;
    types _type;
};

inline
auto vertex_array::push(spl::graphics::vertex v, std::optional<spl::graphics::rgba> c) noexcept -> vertex_array &
{
    _buffer.emplace_back(v.x, v.y, c);
    return *this;
}

inline
void vertex_array::render_on(spl::graphics::viewport img) const noexcept
{
    auto draw_points = [this](spl::graphics::viewport img) {
        for (auto const [x, y, c] : _buffer) {
            img.pixel_noexcept(x,y)=c.value_or(_color);
        }
    };
    auto draw_lines = [this](spl::graphics::viewport img) {
        auto start = _buffer.begin();
        auto gigi  = std::ranges::next(start, 1, _buffer.end());
        while(gigi != _buffer.end())
        {
            auto const [x1, y1, c1] = *start;
            auto const [x2, y2, c2] = *gigi;
            img.draw(spl::graphics::line{{x1,y1}, {x2,y2}, c1.value_or(_color)});
            start = ++gigi;
            gigi  = std::ranges::next(start, 1, _buffer.end());
        }
        if (start != _buffer.end()) {
            auto const [x, y, c] = *start;
            img.pixel_noexcept(x, y) = c.value_or(_color);
        }
    };
    auto draw_lines_strip = [this](spl::graphics::viewport img) {
        auto start = _buffer.begin();
        auto gigi  = std::ranges::next(start, 1, _buffer.end());
        while(gigi != _buffer.end())
        {
            auto const [x1, y1, c1] = *start;
            auto const [x2, y2, c2] = *gigi;
            img.draw(spl::graphics::line{{x1,y1}, {x2,y2}, c1.value_or(_color)});
            start = gigi;
            gigi  = std::ranges::next(start, 1, _buffer.end());
        }
    };
    auto draw_triangles = [this](spl::graphics::viewport ) {

    };
    auto draw_triangles_strip = [this](spl::graphics::viewport ) {

    };
    auto draw_triangles_fan = [this](spl::graphics::viewport ) {

    };
}

} // namespace spl::graphics

#endif
