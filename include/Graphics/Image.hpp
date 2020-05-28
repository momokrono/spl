#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <vector>
#include <cstdint>
// #include <iostream>

#include "bits/Iterators.hpp"
#include "rgba.hpp"

namespace spl::graphics
{
class image
{
public:
    image() : _width{0}, _height{0} {};
    image(int w, int h) : _width{w}, _height{h} {};
    auto access_row(size_t const y) -> spl::graphics::image_range<true>;
    auto access_column(size_t const x) -> spl::graphics::image_range<false>;
    auto pixel(size_t const x, size_t const y) const -> rgba;
    auto pixel(size_t const x, size_t const y) -> rgba &;
    auto get_pixel_iterator(size_t const x, size_t const y) ->std::vector<rgba>::iterator;
private:
    std::vector<rgba> _pixels;
    int _width, _height;
};

} // namespace spl::graphics

#endif