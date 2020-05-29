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
    // auto access_rows() -> spl::graphics::image_rows_cols_range<true>; // range of rows
    // auto access_columns() -> spl::graphics::image_rows_cols_range<false>; // range of columns
    auto access_row(size_t const y)    -> spl::graphics::image_range<true>;
    auto access_column(size_t const x) -> spl::graphics::image_range<false>;
    auto get_pixel_iterator(size_t const x, size_t const y) ->std::vector<rgba>::iterator;

private:
    std::vector<rgba> _pixels;
    size_t _width, _height;
};

} // namespace spl::graphics

#endif
