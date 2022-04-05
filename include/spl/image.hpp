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

    /// @name Constructors
    ///@{
    /// Construct an empty `image`
    image() noexcept : _width{0}, _height{0} {};
    /**
      Construct an `image` of size `w`×`h`, filled with color `fill`

      @param w the width of the image
      @param h the height of the image
      @param fill the color to use to fill the image (default: white)

      \rst
      **Example**::

        auto img = spl::graphics::image{800, 600, spl::graphics::color::red};

      \endrst
     */
    image(index_type w, index_type h, rgba fill = color::white) noexcept
        : _pixels{w * h, fill}, _width{w}, _height{h} {}
    /**
      Construct an `image` of size `w`×`h`, do not fill the image with a color

      @param w the width of the image
      @param h the height of the image

      \rst
      **Example**::

        auto img = spl::graphics::image{spl::graphics::construct_uninitialized, 800, 600};

      \endrst
     * */
    image(construct_uninitialized_t, index_type w, index_type h) noexcept
        : _pixels{w * h}, _width{w}, _height{h} {}

    /**
      Construct an `image` copying a `viewport` or an `image_view`

      @param v The `viewport` that will be copied into the image

      \rst
      **Example**::

        auto source = spl::graphics::image{200, 200};
        auto view = spl::graphics::viewport{source, 0, 0, 100, 100};
        auto new_image = spl::graphics::image{view};

      \endrst
     * */
    template <bool Const2> image(basic_viewport<Const2> v)
        : _pixels(v.begin(), v.end()), _width{v.width()}, _height{v.height()}
    {}
    ///@}

    /// @name Direct element access
    ///@{
    /**
      Retrieves the pixel at the specified offset

      @param x the horizontal offset of the pixel from the left side
      @param y the vertical offset of the pixel from the top side
      @throw spl::graphics::out_of_range
      @return a reference to the specified pixel

      \rst
      **Example**::

        // break out of the loop when an out-of-range pixel is retrieved
        for (auto i = 0ul; i < image.height(); ++i) {
            try {
                auto & pixel = img.pixel(i, i);
                pixel = spl::graphics::color::black;
            }
            except (spl::graphics::out_of_range const & exc) {
                break;
            }
        }

      \endrst
     * */
    auto pixel(index_type const x, index_type const y)       -> reference;
    auto pixel(index_type const x, index_type const y) const -> const_reference;

    /**
      Retrieves the pixel located in the specified vertex

      @param pt the vertex in which the pixel is located
      @throw spl::graphics::out_of_range
      @return a reference to the specified pixel

      \rst
      **Example**::

        auto vertices = get_list_of_points();
        for (spl::graphics::vertex v : vertices) {
            try {
                auto & pixel = img.pixel(v);
                pixel = spl::graphics::color::black;
            }
            except (spl::graphics::out_of_range & exc) {
            }
        }
      \endrst
     * */
    auto pixel(vertex const pt)       -> reference       { return pixel(pt.x, pt.y); }
    auto pixel(vertex const pt) const -> const_reference { return pixel(pt.x, pt.y); }

    /**
      Retrieves the pixel at the specified offset, or a writable garbage pixel if the required one
      does not exists

      @param x the horizontal offset of the pixel from the left side
      @param y the vertical offset of the pixel from the top side
      @return a reference to the specified pixel if it is in range, or to a writable garbage pixel

      \rst
      **Example**::

        // if an out-of-range pixel is retrieved, a reference to a "garbage pixel" is returned
        // instead; writing on that pixel will not have any visible consequence
        for (auto i = 0ul; i < image.height(); ++i) {
            auto & pixel = img.pixel_noexcept(i, i);
            pixel = spl::graphics::color::black;
        }

      \endrst
     * */
    auto pixel_noexcept(index_type const x, index_type const y)       noexcept -> reference;
    auto pixel_noexcept(index_type const x, index_type const y) const noexcept -> const_reference;

    /**
      Retrieves the pixel located in the specified vertex, or a writable garbage pixel if the required
      one does not exists

      @param pt the vertex in which the pixel is located
      @return a reference to the specified pixel if it is in range, or to a writable garbage pixel

      \rst
      **Example**::

        auto vertices = get_list_of_points();
        for (spl::graphics::vertex v : vertices) {
            auto & pixel = img.pixel(v);
            pixel = spl::graphics::color::black;
        }

      \endrst
     * */
    auto pixel_noexcept(vertex const pt)       noexcept -> reference       { return pixel_noexcept(pt.x, pt.y); }
    auto pixel_noexcept(vertex const pt) const noexcept -> const_reference { return pixel_noexcept(pt.x, pt.y); }
    ///@}

    /// @name Row and column iteration
    ///@{
    /**
      Returns a view of the `y`-th row

      @param y the offset of the required row
      @returns an iterable view of the row
     * */
    auto row(index_type const y)            -> row_view;
    auto row(index_type const y) const      -> const_row_view;

    /**
      Returns a view of the `x`-th column

      @param x the offset of the required column
      @returns an iterable view of the column
     * */
    auto column(index_type const x)         -> column_view;
    auto column(index_type const x) const   -> const_column_view;

    /**
     * Returns the range of all the image rows
     * */
    auto rows()                         -> row_range;
    auto rows() const                   -> const_row_range;

    /**
     * Returns the range of all the image columns
     * */
    auto columns()                      -> column_range;
    auto columns() const                -> const_column_range;
    ///@}

    /// @name Iterators
    /// @{
    /// Returns an iterator to the first pixel
    auto begin()        -> iterator       { return _pixels.begin(); }
    auto begin()  const -> const_iterator { return _pixels.begin(); }
    /// Returns a const_iterator to the first pixel
    auto cbegin() const -> const_iterator { return _pixels.cbegin(); }
    /// Returns an iterator to the end of the pixel buffer
    auto end()          -> iterator       { return _pixels.end(); }
    auto end()    const -> const_iterator { return _pixels.end(); }
    /// Returns a const_iterator to the end of the pixel buffer
    auto cend()   const -> const_iterator { return _pixels.cend(); }

    /// Returns an iterator to the pixel in position `x`,`y`
    auto get_pixel_iterator(index_type const x, index_type const y)       -> iterator;
    auto get_pixel_iterator(index_type const x, index_type const y) const -> const_iterator;
    ///@}

    /// @name Capacity
    /// @{
    /// returns the number of columns (unsigned)
    auto width()       const noexcept { return _width; }
    /// returns the number of rows (unsigned)
    auto height()      const noexcept { return _height; }
    /// returns a pair containing the height and the width of the image (both unsigned)
    auto dimensions()  const noexcept { return std::pair{ width(), height() }; }
    /// returns the number of columns (signed)
    auto swidth()      const noexcept { return static_cast<ptrdiff_t>(_width); }
    /// returns the number of rows (signed)
    auto sheight()     const noexcept { return static_cast<ptrdiff_t>(_height); }
    /// returns a pair containing the height and the width of the image (both signed)
    auto sdimensions() const noexcept { return std::pair{ swidth(), sheight() }; }
    /// check wether the image is empty
    bool empty()       const noexcept { return _pixels.empty(); }
    ///@}

    /// @name Drawing
    ///@{
    /**
      Fills an image with a color
      @param c the color the image will be filled with
      @returns `*this`
     * */
    auto fill(rgba const c) & noexcept -> image &;

    // TODO: reference to drawable documentation
    /**
      Draws a `drawable` object on the image
      @tparam Ds any number of types satisfying the concept of `drawable`
      @param objs any number of `drawable` objects
      @returns `*this`
     * */
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
    ///@}

    /// @name General utilities
    ///@{
    /// Direct access to the underlying array
    auto raw_data() const noexcept { return _pixels.data(); }
    /// Saves the image on the filesystem; returns `true` in case of success, `false` otherwise
    bool save_to_file(std::string_view const filename) const;
    // TODO: ref to load_status docs
    /// Loads an image from the filesystem and returns a `load_status`
    auto load_from_file(std::filesystem::path const & filename) -> load_status;
    ///@}

    // TODO: to document?
    /// @name Conversion to a viewport
    ///@{
    explicit operator basic_viewport<false>() & noexcept;
    explicit operator basic_viewport<true>() const & noexcept;
    ///@}

private:
    auto load_ppm(std::filesystem::path const & filename) -> load_status;

    std::vector<rgba> _pixels;
    index_type _width, _height;
};

} // namespace spl::graphics

#endif /* IMAGE_HPP */
