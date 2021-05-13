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

private:
    image_t * _base = nullptr;
    int_fast32_t _x = 0, _y = 0;
    size_t _width = 0, _height = 0;
    // float rotation = 0.f;

public:
    constexpr basic_viewport() = default;
    constexpr basic_viewport(basic_viewport const &) = default;
    constexpr basic_viewport(basic_viewport && other) noexcept
    {
        _base = std::exchange(other._base, nullptr);
        _x = std::exchange(other._x, 0);
        _y = std::exchange(other._y, 0);
        _width  = std::exchange(other._width, 0);
        _height = std::exchange(other._height, 0);
    }

    template <std::integral Int1, std::integral Int2>
    constexpr basic_viewport(
        basic_viewport img, int_fast32_t x_off, int_fast32_t y_off, Int1 w, Int2 h
    ) :
        _base{img._base},
        _x{img._x + x_off}, _y{img._y + y_off},
        _width{static_cast<size_t>(w)}, _height{static_cast<size_t>(h)}
    {}

    constexpr basic_viewport(basic_viewport img, int_fast32_t x_off, int_fast32_t y_off) :
        _base{img._base},
        _x{img._x + x_off}, _y{img._y + y_off},
        _width{img.width()}, _height{img.height()}
    {}

    // TODO: want to disable rvalues for img, how to do it?
    template <std::integral Int1, std::integral Int2>
    explicit
    constexpr basic_viewport(image_t & img, int_fast32_t x_off, int_fast32_t y_off, Int1 w, Int2 h) :
        _base{std::addressof(img)},
        _x{x_off}, _y{y_off},
        _width{static_cast<size_t>(w)}, _height{static_cast<size_t>(h)}
    {}

    explicit
    constexpr basic_viewport(image_t & img, int_fast32_t x_off, int_fast32_t y_off) :
        _base{std::addressof(img)},
        _x{x_off}, _y{y_off},
        _width{img.width() - x_off}, _height{img.height() - y_off}
    {}

    // TODO: want to disable rvalues for img, how to do it?
    constexpr basic_viewport(image_t & img) : basic_viewport{img, 0, 0, img.width(), img.height()} {}

    constexpr basic_viewport(basic_viewport<false> const & v) requires (Const) :
        _base{v._base},
        _x{v._x}, _y{v._y},
        _width{v._width}, _height{v._height}
    {}

    auto pixel(int_fast32_t const x, int_fast32_t const y)       -> reference;
    auto pixel(int_fast32_t const x, int_fast32_t const y) const -> const_reference;
    auto pixel_noexcept(int_fast32_t const x, int_fast32_t const y)       noexcept -> reference;
    auto pixel_noexcept(int_fast32_t const x, int_fast32_t const y) const noexcept -> const_reference;

    auto row(size_t const y)            -> row_view;
    auto row(size_t const y) const      -> const_row_view;

    auto column(size_t const x)         -> column_view;
    auto column(size_t const x) const   -> const_column_view;

    auto rows()                         -> row_range;
    auto rows() const                   -> const_row_range;

    auto columns()                      -> column_range;
    auto columns() const                -> const_column_range;

    auto width()       const noexcept { return _width; }
    auto height()      const noexcept { return _height; }
    auto dimensions()  const noexcept { return std::pair{ width(), height() }; }
    auto swidth()      const noexcept { return static_cast<ptrdiff_t>(_width); }
    auto sheight()     const noexcept { return static_cast<ptrdiff_t>(_height); }
    auto sdimensions() const noexcept { return std::pair{ width(), height() }; }

    auto offset()      const noexcept { return std::pair{_x, _y}; }

    // bool empty()       const noexcept { return _pixels.empty(); }

    auto fill(rgba const c) &  noexcept -> basic_viewport & requires (not Const);
    auto fill(rgba const c) && noexcept -> basic_viewport   requires (not Const);
    template <drawable D>
    auto draw(D && obj) & noexcept -> basic_viewport & requires (not Const)
    {
        if constexpr (spl::detail::has_render_on_member_function<D>) {
            std::forward<D>(obj).render_on(*this);
        } else {
            std::forward<D>(obj)(*this);
        }
        return *this;
    }

    auto base() noexcept -> image_t & { return *_base; }
    auto base() const noexcept -> image_t const & { return *_base; }

    auto begin() noexcept { return iterator{*this}; }
    auto end() noexcept { return iterator{*this, 0, sheight()}; }

    auto begin() const noexcept { return const_iterator{*this}; }
    auto end() const noexcept { return const_iterator{*this, 0, sheight()}; }

    auto cbegin() const noexcept { return const_iterator{*this}; }
    auto cend() const noexcept { return const_iterator{*this, 0, sheight()}; }
};

using viewport = basic_viewport<false>;
using image_view = basic_viewport<true>;

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

