#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <cstdint>
#include <filesystem>

#include "spl/primitives/vertex.hpp"
#include "spl/detail/iterators.hpp"
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
    using const_reference    = value_type const &;
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
    using index_type         = std::size_t;

    // constructors
    image() noexcept : _width{0}, _height{0} {};
    image(index_type w, index_type h, rgba fill = {0, 0, 0, 255}) noexcept
        : _pixels{w * h, fill}, _width{w}, _height{h} {}
    image(construct_uninitialized_t, index_type w, index_type h) noexcept
        : _pixels{w * h}, _width{w}, _height{h} {}
    template <bool Const2> image(basic_viewport<Const2> v)
        : _pixels(v.begin(), v.end()), _width{v.width()}, _height{v.height()} {}

    // direct element access
    auto pixel(index_type const x, index_type const y)       -> reference;
    auto pixel(index_type const x, index_type const y) const -> const_reference;
    auto pixel_noexcept(index_type const x, index_type const y)       noexcept -> reference;
    auto pixel_noexcept(index_type const x, index_type const y) const noexcept -> const_reference;

    auto pixel(vertex const pt)       -> reference       { return pixel(pt.x, pt.y); }
    auto pixel(vertex const pt) const -> const_reference { return pixel(pt.x, pt.y); }
    auto pixel_noexcept(vertex const pt)       noexcept -> reference       { return pixel_noexcept(pt.x, pt.y); }
    auto pixel_noexcept(vertex const pt) const noexcept -> const_reference { return pixel_noexcept(pt.x, pt.y); }

    // iteration
    auto row(index_type const y)            -> row_view;
    auto row(index_type const y) const      -> const_row_view;

    auto column(index_type const x)         -> column_view;
    auto column(index_type const x) const   -> const_column_view;

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

    auto get_pixel_iterator(index_type const x, index_type const y)       -> iterator;
    auto get_pixel_iterator(index_type const x, index_type const y) const -> const_iterator;

    auto width()       const noexcept { return _width; }
    auto height()      const noexcept { return _height; }
    auto dimensions()  const noexcept { return std::pair{ width(), height() }; }
    auto swidth()      const noexcept { return static_cast<ptrdiff_t>(_width); }
    auto sheight()     const noexcept { return static_cast<ptrdiff_t>(_height); }
    auto sdimensions() const noexcept { return std::pair{ swidth(), sheight() }; }
    bool empty()       const noexcept { return _pixels.empty(); }

    // drawing
    auto fill(rgba const c) & noexcept -> image &;

    template <drawable ...Ds>
        requires(sizeof...(Ds) >= 1)
    auto draw(Ds &&... objs) & noexcept -> image &
    {
        auto draw_impl = [this]<drawable D>(D && obj) {
            if constexpr (spl::detail::has_render_on_member_function<D>) {
                std::forward<D>(obj).render_on(*this);
            } else {
                std::forward<D>(obj)(*this);
            }
        };
        (draw_impl(std::forward<Ds>(objs)), ...);
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
    auto load_qoi(std::filesystem::path const & filename) -> load_status;
    bool save_to_ppm(std::string_view const filename) const;
    bool save_to_qoi(std::string_view const filename) const;

    std::vector<rgba> _pixels;
    index_type _width, _height;
};

} // namespace spl::graphics

#endif /* IMAGE_HPP */
