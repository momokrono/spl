#include "Image.hpp"
namespace spl::graphics
{
    auto Image::get_pixel( int const x, int const y )
        -> rgba
    {
        if ( x > _width - 1 or y > _height - 1)
        {
            std::cerr << "trying to access pixel out of boundaries\n";
            return {0xFF};
        } else {
            return _pixels[x][y];
        }
    };

    auto Image::access_row( int const y )
        -> std::vector<std::vector<rgba>>::iterator
    {
        if ( y > _height -1 )
        {
            std::cerr << "trying to access row out of boundaries\n";
            return _pixels.end();
        } else {
            return _pixels.begin() + y;
        }
    }

    auto Image::access_column( int const x )
        -> std::vector<rgba>::iterator
    {
        if ( x > _width -1 )
        {
            std::cerr << "trying to access column out of boundaries\n";
            return _pixels[_width].end();
        } else {
            return _pixels[0].begin() + x;
        }
    }

    auto Image::access_pixel( int const x, int const y )
        -> std::pair<std::vector<std::vector<rgba>>::iterator, std::vector<rgba>::iterator>
    {
        if ( x > _width - 1 or y > _height - 1)
        {
            std::cerr << "trying to access pixel out of boundaries\n";
            return {_pixels.end(), _pixels.at(_width-1).end()};
        } else {
            return {_pixels.begin() + y, _pixels[y].begin() + x };
        }
    }
}; // namespace spl::graphics