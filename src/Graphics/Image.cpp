#include "Image.hpp"
namespace spl::graphics
{
    auto image::get_pixel( int const x, int const y )
        -> rgba
    {
        if ( x > _width - 1 or y > _height - 1)
        {
            std::cerr << "trying to access pixel out of boundaries\n";
<<<<<<< HEAD
            return {0, 0, 0, 0};
=======
            return {0};
>>>>>>> ef9910ba33439b13977be71dd4eb67d3975d7f94
        } else {
            return _pixels[x][y];
        }
    };

    auto image::access_row( int const y )
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

    auto image::access_column( int const x )
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

    auto image::access_pixel( int const x, int const y )
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
