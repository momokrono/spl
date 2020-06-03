/**
 * @author      : momokrono, rbrugo
 * @file        : image.cpp
 * @created     :
 * @license     : MIT
 */

#include "Graphics/Image.hpp"
#include "bits/exceptions.hpp"

#include <fmt/core.h>

namespace spl::graphics
{

auto image::get_pixel_iterator(size_t const x, size_t const y)
    ->std::vector<rgba>::iterator
{
    if ( x > _width - 1 or y > _height - 1) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.begin()+(x + y * _width);
}

auto image::rows()
    -> spl::graphics::image_range<true>
{
    return {row(0), height()};
}

auto image::columns()
    -> spl::graphics::image_range<false>
{
    return {column(0), width()};
}

auto image::row(size_t const y)
    -> spl::graphics::row_col_range<true>
{
    auto it = get_pixel_iterator(0, y);
    return {it, _width};
}

auto image::column(size_t const x)
    -> spl::graphics::row_col_range<false>
{
    auto it = get_pixel_iterator(x, 0);
    return {{it, _width}, _height};
}

auto image::pixel(size_t const x, size_t const y) const
    -> rgba
{
    if ( x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x+y*_width);
}

auto image::pixel(size_t const x, size_t const y)
    -> rgba &
{
    if ( x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x + y * _width);
}

} // namespace spl::graphics
