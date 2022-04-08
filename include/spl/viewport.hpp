/**
 * @author      : rbrugo, momokrono
 * @file        : viewport
 * @created     : Sunday Mar 21, 2021 21:54:32 CET
 * @license     : MIT
 * */

#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "image.hpp"

namespace spl::graphics
{
template <bool, bool>
struct viewport_iterator;

template <bool Const>
class basic_viewport;

using viewport = basic_viewport<false>;
using image_view = basic_viewport<true>;


template <bool Const>
class basic_viewport
{
    using image_t = std::conditional_t<Const, image const, image>;
    friend class basic_viewport<not Const>;

public:
    using value_type         = typename image_t::value_type;
    using reference          = std::conditional_t<Const, value_type, value_type &>;
    using const_reference    = typename image_t::const_reference;
    using iterator           = viewport_iterator<Const, Const>;
    using const_iterator     = viewport_iterator<true, Const>;
    using row_view           = row_col_range<true,  Const>;
    using const_row_view     = row_col_range<true,  true>;
    using column_view        = row_col_range<false, Const>;
    using const_column_view  = row_col_range<false, true>;
    using row_range          = image_range<true,  Const>;
    using const_row_range    = image_range<true,  true>;
    using column_range       = image_range<false, Const>;
    using const_column_range = image_range<false, true>;
    using index_type         = vertex::value_type;

private:
    image_t * _base = nullptr;
    index_type _x = 0, _y = 0;
    size_t _width = 0, _height = 0;
    // float rotation = 0.f;

public:
    /// @name Constructors
    /// @{
    /// Construct an empty `viewport`
    constexpr basic_viewport() = default;
    /// Copy constructor
    constexpr basic_viewport(basic_viewport const &) = default;
    /// Move constructor
    constexpr basic_viewport(basic_viewport && other) noexcept
    {
        _base = std::exchange(other._base, nullptr);
        _x = std::exchange(other._x, 0);
        _y = std::exchange(other._y, 0);
        _width  = std::exchange(other._width, 0);
        _height = std::exchange(other._height, 0);
    }

    // TODO: what happens if w and h are greater than the image size?
    /**
      Construct a `viewport` by clipping another

      @param img the source `viewport`
      @param x_off the horizontal offset from the left side of the original `viewport`
      @param y_off the vertical offset from the top side of the original `viewport`
      @param w the width of the new `viewport`
      @param h the height of the new `viewport`

     * */
    template <std::integral Int1, std::integral Int2>
    constexpr basic_viewport(
        basic_viewport img, index_type x_off, index_type y_off, Int1 w, Int2 h
    ) :
        _base{img._base},
        _x{img._x + x_off}, _y{img._y + y_off},
        _width{static_cast<size_t>(w)}, _height{static_cast<size_t>(h)}
    {}

    /**
      Construct a `viewport` by clipping another

      @param img the source `viewport`
      @param x_off the horizontal offset from the left side of the original `viewport`
      @param y_off the vertical offset from the top side of the original `viewport`
     * */
    constexpr basic_viewport(basic_viewport img, index_type x_off, index_type y_off) :
        _base{img._base},
        _x{img._x + x_off}, _y{img._y + y_off},
        _width{img.width() - x_off}, _height{img.height() - y_off}
    {}

    // TODO: want to disable rvalues for img, how to do it?
    /**
      Construct a `viewport` by clipping an `image`

      @param img the source `viewport`
      @param x_off the horizontal offset from the left side of the original `viewport`
      @param y_off the vertical offset from the top side of the original `viewport`
      @param w the width of the new `viewport`
      @param h the height of the new `viewport`
     * */
    template <std::integral Int1, std::integral Int2>
    explicit
    constexpr basic_viewport(image_t & img, index_type x_off, index_type y_off, Int1 w, Int2 h) :
        _base{std::addressof(img)},
        _x{x_off}, _y{y_off},
        _width{static_cast<size_t>(w)}, _height{static_cast<size_t>(h)}
    {}

    /**
      Construct a `viewport` by clipping an `image`

      @param img the source `viewport`
      @param x_off the horizontal offset from the left side of the original `viewport`
      @param y_off the vertical offset from the top side of the original `viewport`
     * */
    explicit
    constexpr basic_viewport(image_t & img, index_type x_off, index_type y_off) :
        _base{std::addressof(img)},
        _x{x_off}, _y{y_off},
        _width{img.width() - x_off}, _height{img.height() - y_off}
    {}

    // TODO: want to disable rvalues for img, how to do it?
    /// Construct a `viewport` of a whole `image`
    constexpr basic_viewport(image_t & img) : basic_viewport{img, 0, 0, img.width(), img.height()} {}

    /// Construct an `image_view` from a `viewport`
    constexpr basic_viewport(viewport const & v) requires (Const) :
        _base{v._base},
        _x{v._x}, _y{v._y},
        _width{v._width}, _height{v._height}
    {}
    /// @}

    /// @name Direct element access
    /// @{
    /**
      Retrieves the pixel at the specified offset

      @param x the horizontal offset of the pixel from the left side
      @param y the vertical offset of the pixel from the top side
      @throw spl::graphics::out_of_range if the pixel is not contained in the underlying image
      @return a reference to the specified pixel

      \rst
      **Example**::

        // break out of the loop when an out-of-range pixel is retrieved
        void draw_parable(spl::graphics::viewport v)
        {
            auto x0 = v.swidth() / 2;
            for (int dx = 0; dx < x0; ++dx) {
                try {
                    v.pixel(x0 + dx, dx * dx) = spl::graphics::color::black;
                    v.pixel(x0 - dx, dx * dx) = spl::graphics::color::black;
                } except (spl::graphics::out_of_range const & exc) {
                    break;
                }
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

        void draw_points(spl::graphics::viewport vp, auto const & vertices)
        {
            for (spl::graphics::vertex v : vertices) {
                try {
                    auto & pixel = vp.pixel(v);
                    pixel = spl::graphics::color::black;
                }
                except (spl::graphics::out_of_range & exc) {
                }
            }
        }
      \endrst
     * */
    auto pixel(vertex const pt)       -> reference       { return pixel(pt.x, pt.y); }
    auto pixel(vertex const pt) const -> const_reference { return pixel(pt.x, pt.y); }

    /**
      Retrieves the pixel at the specified offset, or a writable garbage pixel if the required one
      is outside of the underlying image

      @param x the horizontal offset of the pixel from the left side of the viewport
      @param y the vertical offset of the pixel from the top side of the viewport
      @return a reference to the specified pixel if it is in range, or to a writable garbage pixel

      \rst
      **Example**::

        // break out of the loop when an out-of-range pixel is retrieved
        void draw_parable(spl::graphics::viewport v) noexcept
        {
            auto x0 = v.swidth() / 2;
            for (int dx = 0; dx < x0; ++dx) {
                v.pixel_noexcept(x0 + dx, dx * dx) = spl::graphics::color::black;
                v.pixel_noexcept(x0 - dx, dx * dx) = spl::graphics::color::black;
            }
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

        void draw_points(spl::graphics::viewport vp, auto const & vertices) noexcept
        {
            for (spl::graphics::vertex v : vertices) {
                auto & pixel = vp.pixel_noexcept(v);
                pixel = spl::graphics::color::black;
            }
        }

      \endrst
     * */
    auto pixel_noexcept(vertex const pt)       noexcept -> reference       { return pixel_noexcept(pt.x, pt.y); }
    auto pixel_noexcept(vertex const pt) const noexcept -> const_reference { return pixel_noexcept(pt.x, pt.y); }
    /// @}

   /// @name Row and column iteration
   /// @{
   /**
     Returns a view of the `y`-th row of the viewport

     @param y the offset of the required row
     @returns an iterable view of the row
    * */
   auto row(size_t const y)            -> row_view;
   auto row(size_t const y) const      -> const_row_view;

   /**
     Returns a view of the `x`-th column of the viewport

     @param x the offset of the required column
     @returns an iterable view of the column
    * */
   auto column(size_t const x)         -> column_view;
   auto column(size_t const x) const   -> const_column_view;

   /**
    * Returns the range of all the viewport rows
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
   /// Returns an iterator to the first pixel of the viewport
   auto begin() noexcept { return iterator{*this}; }
   auto begin() const noexcept { return const_iterator{*this}; }
   /// Returns a const_iterator to the first pixel of the viewport
   auto cbegin() const noexcept { return const_iterator{*this}; }
   /// Returns an iterator to the end of the viewport
   auto end() noexcept { return iterator{*this, 0, sheight()}; }
   auto end() const noexcept { return const_iterator{*this, 0, sheight()}; }
   /// Returns a const_iterator to the end of the viewport
   auto cend() const noexcept { return const_iterator{*this, 0, sheight()}; }
   /// @}

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
   auto sdimensions() const noexcept { return std::pair{ width(), height() }; }
   // bool empty()       const noexcept { return _pixels.empty(); }
   /// @}

   /// @name Drawing
   /// @{
   /**
     Fills the covered area of the underlying image with a color
     @param c the color the image will be filled with
     @returns `*this`
    * */
   auto fill(rgba const c) &  noexcept -> basic_viewport & requires (not Const);
   auto fill(rgba const c) && noexcept -> basic_viewport   requires (not Const);

   // TODO: reference to drawable documentation
   /**
     Draws a `drawable` object on the viewport
     @tparam Ds any number of types satisfying the concept of `drawable`
     @param objs any number of `drawable` objects
     @returns `*this`
    * */
   template <drawable ...Ds>
       requires(sizeof...(Ds) >= 1)
   auto draw(Ds &&... objs) & noexcept -> basic_viewport & requires (not Const)
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
   /// @}

   /// @name General utilities
   /// @{
   /// Direct access to the underlying image
   auto base() noexcept -> image_t & { return *_base; }
   auto base() const noexcept -> image_t const & { return *_base; }

   /// Get a pair containing the offset of the top-left corner of the viewport
   auto offset()      const noexcept { return std::pair{_x, _y}; }
   /// @}
};

template <bool Const, bool BasicConst>
struct viewport_iterator
{
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type = std::conditional_t<not Const, rgba, rgba const>;
    using reference = std::conditional_t<Const or BasicConst, value_type, value_type &>;
    using const_reference = value_type const &;

    using basic_viewport_t = basic_viewport<BasicConst>;
    using viewport_t = std::conditional_t<not Const, basic_viewport_t, basic_viewport_t const>;

private:
    viewport_t * _base = nullptr;
    std::ptrdiff_t _x = 0;
    std::ptrdiff_t _y = 0;

public:
    viewport_iterator() = default;
    viewport_iterator(viewport_t & v, std::ptrdiff_t x, std::ptrdiff_t y) noexcept
        : _base{std::addressof(v)}, _x{x}, _y{y}
    {}

    viewport_iterator(viewport_t & v) noexcept : viewport_iterator{v, 0, 0} {}
    auto operator*()       -> reference { return _base->pixel(_x, _y); }
    auto operator*() const -> reference { return _base->pixel(_x, _y); }

    auto operator++() noexcept
        -> viewport_iterator &
    {
        ++_x;
        if (_x == _base->swidth()) {
            _x = 0;
            _y = std::min(_y + 1, _base->sheight());
        }
        return *this;
    }

    auto operator++(int) const noexcept
        -> viewport_iterator
    {
        auto copy = *this;
        ++*this;
        return copy;
    }

    bool operator==(viewport_iterator const &) const noexcept = default;
};

static_assert(std::input_iterator<viewport_iterator<true, true>>); // const iterator to const view
static_assert(std::input_iterator<viewport_iterator<true, false>>); // const iterator to mutable view
static_assert(std::input_iterator<viewport_iterator<false, true>>); // mutable iterator to const view
static_assert(std::input_iterator<viewport_iterator<false, false>>); // mutable iterator to mutable view
static_assert(not std::indirectly_writable<viewport_iterator<false, true>, rgba>);  // mutable iterator to const view
static_assert(not std::indirectly_writable<viewport_iterator<true, true>, rgba>);  // const iterator to const view
static_assert(std::indirectly_writable<viewport_iterator<false, false>, rgba>);  // mutable iterator to mutable view
static_assert(not std::indirectly_writable<viewport_iterator<true, false>, rgba>);  // const iterator to mutable view
static_assert(std::weakly_incrementable<viewport_iterator<true, false>>);  // const iterator to mutable view
static_assert(std::weakly_incrementable<viewport_iterator<false, false>>);  // mutable iterator to mutable view
static_assert(std::weakly_incrementable<viewport_iterator<true, true>>);  // const iterator to const view
static_assert(std::weakly_incrementable<viewport_iterator<false, true>>);  // mutable iterator to const view

} // namespace spl::graphics

#endif /* VIEWPORT_HPP */
