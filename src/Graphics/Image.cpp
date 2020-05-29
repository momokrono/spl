/**
 * @author      : momokrono, rbrugo
 * @file        : image.cpp
 * @created     :
 * @license     : MIT
 */

#include "Graphics/Image.hpp"
#include <fmt/core.h>

namespace spl::graphics
{

struct out_of_range : std::exception
{
    std::string message;
    out_of_range(size_t x, size_t y, size_t w, size_t h)
    {
        message = fmt::format("out of range: required ({}, {}) but size is ({}, {})", x, y, w, h);
    }

    auto what() const noexcept -> char const * override 
    { return message.c_str(); }
};

auto image::get_pixel_iterator(size_t const x, size_t const y)
    ->std::vector<rgba>::iterator
{
    if ( x > _width - 1 or y > _height - 1) {
        throw out_of_range{x, y, _width, _height};
    }
    return _pixels.begin()+(x + y * _width);
}

auto image::access_row(size_t const y)
    -> spl::graphics::image_range<true>
{
    auto it = get_pixel_iterator(0, y);
    return {it, _width};
}

auto image::access_column(size_t const x)
    -> spl::graphics::image_range<false>
{
    auto it = get_pixel_iterator(x, 0);
    return {{it, _width}, _height};
}

auto image::pixel(size_t const x, size_t const y) const
    -> rgba
{
    if ( x > _width - 1 or y > _height - 1) {
        throw out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x+y*_width);
}

auto image::pixel(size_t const x, size_t const y)
    -> rgba &
{
    if ( x > _width - 1 or y > _height - 1) {
        throw out_of_range{x, y, _width, _height};
    }
    return _pixels.at(x + y * _width);
}

} // namespace spl::graphics
