#include "Image.hpp"
#include <iostream>

namespace spl::graphics
{

struct out_of_range : std::exception
{
    auto what() -> char const * { return "out of range"; }
};

auto image::get_pixel_iterator(size_t const x, size_t const y)
    ->std::vector<rgba>::iterator
{
    if ( x > _width - 1 or y > _height - 1)
    {
        std::cerr << "trying to access pixel out of boundaries\n";
        return _pixels.end();
    }
    return _pixels.begin()+(x+y*_width);
};

auto image::access_row(size_t const y)
    -> spl::graphics::image_range<true>
{
    auto it = get_pixel_iterator(0,y);
    return {it, _width};
};

auto image::access_column(size_t const x)
    -> spl::graphics::image_range<false>
{
    auto it = get_pixel_iterator(x,0);
    return {{it, _width}, _height};
};

auto image::pixel(size_t const x, size_t const y) const
    -> rgba
{
    if ( x > _width - 1 or y > _height - 1)
    {
        throw out_of_range();
    }
    return _pixels.at(x+y*_width);
};

auto image::pixel(size_t const x, size_t const y)
    -> rgba &
{
    if ( x > _width - 1 or y > _height - 1)
    {
        throw out_of_range();
    }
    return _pixels.at(x+y*_width);
};

}; // namespace spl::graphics
