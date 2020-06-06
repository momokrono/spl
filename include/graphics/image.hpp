#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <vector>
#include <cstdint>
#include <filesystem>

#include "bits/iterators.hpp"
#include "rgba.hpp"

namespace spl::graphics
{

enum class load_status : uint8_t { success, file_not_found, failure };

struct construct_uninitialized_t {};

inline constexpr
auto construct_uninitialized = construct_uninitialized_t{};

class image
{
public:
    // constructors
    image() : _width{0}, _height{0} {};
    image(size_t w, size_t h) : _pixels{w * h, {0, 0, 0, 255}}, _width{w}, _height{h} {}
    image(construct_uninitialized_t, size_t w, size_t h) : _pixels{w * h}, _width{w}, _height{h} {}

    // direct element access
    auto pixel(size_t const x, size_t const y) const -> rgba;
    auto pixel(size_t const x, size_t const y) -> rgba &;

    // iteration
    auto rows()       -> spl::graphics::image_range<true, false>;
    auto rows() const -> spl::graphics::image_range<true, true>;
    auto columns()       -> spl::graphics::image_range<false, false>;
    auto columns() const -> spl::graphics::image_range<false, true>;
    auto row(size_t const y)       -> spl::graphics::row_col_range<true, false>;
    auto row(size_t const y) const -> spl::graphics::row_col_range<true, true>;
    auto column(size_t const x)       -> spl::graphics::row_col_range<false, false>;
    auto column(size_t const x) const -> spl::graphics::row_col_range<false, true>;
    auto get_pixel_iterator(size_t const x, size_t const y)       -> std::vector<rgba>::iterator;
    auto get_pixel_iterator(size_t const x, size_t const y) const -> std::vector<rgba>::const_iterator;

    auto width()      const noexcept { return _width; }
    auto height()     const noexcept { return _height; }
    auto dimensions() const noexcept { return std::pair{ width(), height() }; }

    // drawing
    auto fill(rgba const c) noexcept -> image &;

    // utils
    auto raw_data()   const noexcept { return _pixels.data(); }
    bool save_to_file(std::string_view const filename) const;
    auto load_from_file(std::filesystem::path const & filename) -> load_status;
private:
    std::vector<rgba> _pixels;
    size_t _width, _height;
};

} // namespace spl::graphics

#endif
