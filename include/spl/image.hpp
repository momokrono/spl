#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <vector>
#include <cstdint>
#include <filesystem>

#include "spl/bits/iterators.hpp"
#include "drawable.hpp"
#include "rgba.hpp"

namespace spl::graphics
{

enum class load_status : uint8_t { success, file_not_found, failure };

struct construct_uninitialized_t {};

constexpr inline
auto construct_uninitialized = construct_uninitialized_t{};

template <bool Const>
class basic_viewport;

class image
{
    friend class basic_viewport<true>;
    friend class basic_viewport<false>;
    static inline rgba _garbage_pixel;
public:
    using value_type         = rgba;
    using reference          = value_type &;
    using const_reference    = value_type; // const &
    using iterator           = std::vector<value_type>::iterator;
    using const_iterator     = std::vector<value_type>::const_iterator;
	using row_view           = spl::graphics::row_col_range<true, false>;
	using const_row_view     = spl::graphics::row_col_range<true, true>;
	using column_view        = spl::graphics::row_col_range<false, false>;
	using const_column_view  = spl::graphics::row_col_range<false, true>;
	using row_range          = spl::graphics::image_range<true, false>;     // FIXME currently not a range
    using column_range       = spl::graphics::image_range<false, false>;
    using const_row_range    = spl::graphics::image_range<true, true>;
    using const_column_range = spl::graphics::image_range<false, true>;

    // constructors
    image() noexcept : _width{0}, _height{0} {};
    image(size_t w, size_t h, rgba fill = {0, 0, 0, 255}) noexcept : _pixels{w * h, fill}, _width{w}, _height{h} {}
    image(construct_uninitialized_t, size_t w, size_t h) noexcept : _pixels{w * h}, _width{w}, _height{h} {}
    template <bool Const2> image(basic_viewport<Const2> v) : _pixels(v.begin(), v.end()), _width{v.width()}, _height{v.height()} {}

    // direct element access
    auto pixel(size_t const x, size_t const y)       -> reference;
    auto pixel(size_t const x, size_t const y) const -> const_reference;
    auto pixel_noexcept(size_t const x, size_t const y)       noexcept -> reference;
    auto pixel_noexcept(size_t const x, size_t const y) const noexcept -> const_reference;

    // iteration
    auto row(size_t const y)            -> row_view;
    auto row(size_t const y) const      -> const_row_view;

    auto column(size_t const x)         -> column_view;
    auto column(size_t const x) const   -> const_column_view;

    auto rows()                         -> row_range;
    auto rows() const                   -> const_row_range;

    auto columns()                      -> column_range;
    auto columns() const                -> const_column_range;

    auto begin()        -> iterator       { return _pixels.begin(); }
    auto begin()  const -> const_iterator { return _pixels.begin(); }
    auto cbegin() const -> const_iterator { return _pixels.cbegin(); }
    auto end()          -> iterator       { return _pixels.end(); }
    auto end()    const -> const_iterator { return _pixels.end(); }
    auto cend()   const -> const_iterator { return _pixels.cend(); }

    auto get_pixel_iterator(size_t const x, size_t const y)       -> iterator;
    auto get_pixel_iterator(size_t const x, size_t const y) const -> const_iterator;

    auto width()       const noexcept { return _width; }
    auto height()      const noexcept { return _height; }
    auto dimensions()  const noexcept { return std::pair{ width(), height() }; }
    auto swidth()      const noexcept { return static_cast<ptrdiff_t>(_width); }
    auto sheight()     const noexcept { return static_cast<ptrdiff_t>(_height); }
    auto sdimensions() const noexcept { return std::pair{ width(), height() }; }
    bool empty()       const noexcept { return _pixels.empty(); }

    // drawing
    auto fill(rgba const c) & noexcept -> image &;
    template <drawable D>
    auto draw(D && obj) & -> image &
    {
        if constexpr (spl::detail::has_render_on_member_function<D>) {
            std::forward<D>(obj).render_on(*this);
        } else {
            std::forward<D>(obj)(*this);
        }
        return *this;
    }

    // utils
    auto raw_data()   const noexcept { return _pixels.data(); }
    bool save_to_file(std::string_view const filename) const;
    auto load_from_file(std::filesystem::path const & filename) -> load_status;

    // viewport
    explicit operator basic_viewport<false>() & noexcept;
    explicit operator basic_viewport<true>() const & noexcept;

private:
    auto load_ppm(std::filesystem::path const & filename) -> load_status;

    std::vector<rgba> _pixels;
    size_t _width, _height;
};

} // namespace spl::graphics

#endif
