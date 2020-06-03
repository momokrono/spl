#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <vector>
#include <cstdint>

#include "bits/Iterators.hpp"
#include "rgba.hpp"

namespace spl::graphics
{

class image
{
public:
    // constructors
    image() : _width{0}, _height{0} {};
    image(size_t w, size_t h) : _pixels{w * h, {0, 0, 0, 0}}, _width{w}, _height{h} {}

    // direct element access
    auto pixel(size_t const x, size_t const y) const -> rgba;
    auto pixel(size_t const x, size_t const y) -> rgba &;

    // iteration
    // TODO: const overload
    auto rows() -> spl::graphics::image_range<true>;
    auto columns() -> spl::graphics::image_range<false>;
    auto row(size_t const y)    -> spl::graphics::row_col_range<true>;
    auto column(size_t const x) -> spl::graphics::row_col_range<false>;
    auto get_pixel_iterator(size_t const x, size_t const y) -> std::vector<rgba>::iterator;

    auto width()      const noexcept { return _width; }
    auto height()     const noexcept { return _height; }
    auto dimensions() const noexcept { return std::pair{ width(), height() }; }

private:
    std::vector<rgba> _pixels;
    size_t _width, _height;
};

} // namespace spl::graphics

#endif
