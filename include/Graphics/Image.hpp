#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include<vector>
#include <cstdint>
// #include <iostream>

#include "bits/Iterators.hpp"

namespace spl::graphics
{

struct rgba
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class image
{
public:
    Image() : _width{0}, _height{0} {};
    Image( int w, int h ) : _width{w}, _height{h} {};
    auto get_pixel( int const x, int const y)  -> rgba;
    auto access_row( int const y ) -> std::vector<rgba>::iterator;
    auto access_column( int const x ) -> std::vector<rgba>::iterator;
    auto access_pixel( int const x, int const y ) -> std::pair<std::vector<std::vector<rgba>>::iterator, std::vector<rgba>::iterator>;
private:
    std::vector<rgba> _pixels;
    int _width, _height;
};

} // namespace spl::graphics

#endif