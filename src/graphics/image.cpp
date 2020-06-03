/**
 * @author      : momokrono, rbrugo
 * @file        : image.cpp
 * @created     :
 * @license     : MIT
 */

#include "graphics/Image.hpp"
#include "bits/exceptions.hpp"

#include <fstream>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include "stb_image_write.h"

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

auto image::fill(rgba const c) noexcept
    -> image &
{
    std::ranges::fill(_pixels, c);
    return *this;
}

bool image::save_to_file(std::string_view const filename) const
{
    if (filename.ends_with(".bmp")) {
        return stbi_write_bmp(filename.data(), width(), height(), 4, raw_data()) == 1;
    }
    if (filename.ends_with(".png")) {
        return stbi_write_png(filename.data(), width(), height(), 4, raw_data(), 0) == 1;
    }
    if (filename.ends_with(".jpg")) {
        return stbi_write_jpg(filename.data(), width(), height(), 4, raw_data(), 90) == 1;
    }
    if (filename.ends_with(".ppm")) {
        auto sink = std::ofstream{filename.data()};
        if (not sink) {
            return false;
        }

        fmt::print(sink, "P3\n{} {}\n255\n", width(), height());

        for (size_t j = 0; j < height(); ++j) {
            for (size_t i = 0; i < width(); ++i) {
                auto const [r, g, b, a] = pixel(i, j);
                fmt::print(sink, "{} {} {}\n", a * r / 255, a * g / 255, a * b / 255);
            }
            fmt::print(sink, "\n");
        }

        return true;
    }
#if 0
    if (filename.ends_with(".pam")) {
        auto sink = std::ofstream{filename.data()};
        if (not sink) {
            return false;
        }

        fmt::print(sink, "P7\nWIDTH {}\nHEIGHT {}\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n", width(), height());

        for (size_t j = 0; j < height(); ++j) {
            for (size_t i = 0; i < width(); ++i) {
                auto const [r, g, b, a] = pixel(i, j);
                fmt::print(sink, "{} {} {} {}\n", r, g, b, a);
            }
        }

        return true;
    }
#endif
    return false;
}

} // namespace spl::graphics
