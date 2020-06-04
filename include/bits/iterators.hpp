#ifndef _ITERATORS_HPP_
#define _ITERATORS_HPP_

#include <compare>
#include <vector>
#include <ranges>

#include "bits/exceptions.hpp"
#include "graphics/rgba.hpp"

namespace spl::graphics
{
class image;

namespace detail
{
template<bool is_row>
struct iter_step;

template <>
struct iter_step<true>
{
    iter_step(size_t const = 1) {}
};

template <>
struct iter_step<false>
{
    iter_step(size_t const s = 1) noexcept : step{s} {}
    std::size_t step;
};
} // namespace detail

template <bool is_row, bool is_const>
class row_col_iter : detail::iter_step<is_row>
{
    using base = detail::iter_step<is_row>;
public:
    friend class row_col_iter<is_row, true>;
    using iterator          = std::conditional_t<is_const, std::vector<rgba>::const_iterator, std::vector<rgba>::iterator>;
    using iterator_category = iterator::iterator_category;
    using difference_type   = iterator::difference_type;
    using value_type        = iterator::value_type;
    using reference         = std::conditional_t<is_const, value_type const &, value_type &>;
    using const_reference   = value_type const &;
    using pointer           = iterator::pointer;

    row_col_iter() noexcept : base{1} {}
    row_col_iter(row_col_iter const &) noexcept = default;
    row_col_iter(row_col_iter      &&) noexcept = default;

    row_col_iter(iterator i) noexcept : _it{i} {}
    row_col_iter(size_t const s) noexcept : base{s} {}
    row_col_iter(iterator i, size_t const s) noexcept : base{s}, _it{i} {}

    explicit row_col_iter(row_col_iter<is_row, false> const & other) noexcept requires is_const : _it{other._it}
    {
        if constexpr (not is_row) {
            base::step = other.step;
        }
    }
    auto operator=(row_col_iter const & other) noexcept -> row_col_iter & = default;
    auto operator=(row_col_iter      && other) noexcept -> row_col_iter & = default;

    auto operator=(row_col_iter<is_row, false> const & other) noexcept requires is_const
    {
        _it = other._it;
        if constexpr (not is_row)
        {
            base::step = other.step;
        }
    }

    auto operator+=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it += n;
        } else {
            _it += n * base::step;
        }
        return *this;
    }

    auto operator-=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it += n;
        } else {
            _it += n * base::step;
        }
        return *this;
    }

    auto operator+(difference_type const n) const noexcept
        -> row_col_iter
    {
        auto r = *this;
        r += n;
        return r;
    }
    friend auto operator+(difference_type const n, row_col_iter it) noexcept
    { return it += n; }

    auto operator-(difference_type const n) const noexcept
        -> row_col_iter
    {
        auto r = *this;
        r -= n;
        return r;
    }

    auto operator-(row_col_iter const a) const noexcept
        -> difference_type
    {
        if constexpr (is_row) {
            return _it - a._it;
        } else {
            return (_it - a._it) / base::step;
        }
    }
    friend auto operator-(difference_type const n, row_col_iter it) noexcept
    { return it -= n; }

    auto operator++() noexcept
        -> row_col_iter&
    {
        if constexpr (is_row) {
            ++_it;
        } else {
            _it += base::step;
        }
        return *this;
    }

    auto operator++(int) noexcept
        -> row_col_iter
    {
        auto r = *this;
        ++*this;
        return r;
    }

    auto operator--() noexcept
        -> row_col_iter&
    {
        if constexpr (is_row) {
            --_it;
        } else {
            _it -= base::step;
        }
        return *this;
    }

    auto operator--(int)
        -> row_col_iter
    {
        auto r = *this;
        --*this;
        return r;
    }

    auto operator[](difference_type const n) noexcept -> reference
    { return _it[n]; }

    auto operator[](difference_type const n) const noexcept -> const_reference
    { return _it[n]; }

    friend auto operator== (row_col_iter const lhs, row_col_iter const rhs) noexcept
    { return lhs._it == rhs._it; }
    friend auto operator<=>(row_col_iter const lhs, row_col_iter const rhs) noexcept
    { return lhs._it <=> rhs._it; }
    friend auto operator== (row_col_iter const lhs, row_col_iter<is_row, not is_const> const rhs) noexcept
    { return lhs._it == rhs._it; }
    friend auto operator<=>(row_col_iter const lhs, row_col_iter<is_row, not is_const> const rhs) noexcept
    { return lhs._it <=> rhs._it; }

    auto operator*()       -> decltype(auto)
    { return *_it; }

    auto operator*() const -> decltype(auto)
    { return *_it; }

    auto operator->()
    { return std::addressof(*_it); }

    auto operator->() const
    { return std::addressof(*_it); }

private:
    iterator _it;
};

template <bool is_row, bool is_const>
class row_col_range
{
public:
    friend class row_col_range<is_row, true>;

    using value_type      = rgba;
    using iterator        = row_col_iter<is_row, is_const>;
    using const_iterator  = row_col_iter<is_row, true>;
    using reference       = rgba &;
    using const_reference = rgba const &;
    using difference_type = ptrdiff_t;

private:
    iterator _begin;
    size_t _length = 0;

public:
    row_col_range() noexcept = default;
    row_col_range(row_col_range const &) noexcept = default;
    row_col_range(row_col_range      &&) noexcept = default;

    row_col_range(iterator const b, size_t const c) noexcept : _begin{b}, _length{c} {}

    row_col_range(row_col_range<is_row, false> const & other) requires is_const :
        _begin{other._begin}, _length{other._length} {}

    auto operator=(row_col_range const &) noexcept -> row_col_range & = default;
    auto operator=(row_col_range      &&) noexcept -> row_col_range & = default;

    auto operator=(row_col_range<is_row, false> const other) noexcept
        -> row_col_range
        requires is_const
    {
        _begin = other._begin;
        _length = other._length;
        return *this;
    }

    iterator begin() noexcept { return _begin; };
    iterator end()   noexcept { return _begin + _length; };
    const_iterator begin()  const noexcept { return static_cast<const_iterator>(_begin); };
    const_iterator end()    const noexcept { return _begin + _length; };
    const_iterator cbegin() const noexcept { return static_cast<const_iterator>(_begin); };
    const_iterator cend()   const noexcept { return _begin + _length; };

    auto operator[](size_t const n)       noexcept ->       rgba & { return *(_begin + n); }
    auto operator[](size_t const n) const noexcept -> rgba const & { return *(_begin + n); }
    auto at(size_t const n)
        -> rgba &
    {
        if (n >= _length) {
            throw spl::out_of_range(n, _length);
        }
        return *(_begin + n);
    }

    auto at(size_t const n) const
        -> const_reference
    {
        if (n >= _length) {
            throw spl::out_of_range(n, _length);
        }
        return *(_begin + n);
    }


    auto operator+=(size_t const n) noexcept
        -> row_col_range &
    {
        if constexpr (is_row) {
            _begin += _length * n;
        } else {
            _begin += n;
        }
        return *this;
    }
    auto operator+(size_t const n) const noexcept { auto copy = *this; return copy += n; }
    auto operator++() noexcept -> row_col_range & { return *this += 1; }
    auto operator++(int) noexcept { auto copy = *this; ++*this; return copy; }

    auto operator-=(size_t const n) noexcept -> row_col_range & { return *this += -n; }
    auto operator-(size_t const n) const noexcept { return *this + -n; }
    auto operator--() noexcept -> row_col_range & { return *this += -1; }
    auto operator--(int) noexcept { auto copy = *this; --*this; return copy; }

    auto size() const noexcept { return _length; }

    friend bool operator==(row_col_range const, row_col_range const) noexcept = default;
};

template <bool is_row, bool is_const>
class image_range
{
public:
    using value_type      = row_col_range<is_row, is_const>;
    using iterator        = row_col_range<is_row, is_const>;
    using const_iterator  = row_col_range<is_row, is_const>;
    using reference       = value_type &;
    using const_reference = value_type const &;

private:
    iterator _begin;
    size_t   _length = 0;

public:
    image_range() noexcept = default;
    image_range(iterator const b, size_t const c) noexcept : _begin{b}, _length{c} {}
    iterator begin()  const noexcept { return _begin; }
    iterator end()    const noexcept { return _begin + _length; }
    const_iterator cbegin() const noexcept { return _begin; }
    const_iterator cend()   const noexcept { return _begin + _length; }

    auto size() const noexcept { return _length; }

    friend bool operator==(image_range const, image_range const) noexcept = default;
};

} // namespace spl::graphics

namespace std::ranges
{
template <bool is_row, bool is_const>
inline constexpr bool enable_view<spl::graphics::row_col_range<is_row, is_const>> = true;
template <bool is_row, bool is_const>
inline constexpr bool enable_borrowed_range<spl::graphics::row_col_range<is_row, is_const>> = true;
template <bool is_row, bool is_const>
inline constexpr bool enable_view<spl::graphics::image_range<is_row, is_const>> = true;
template <bool is_row, bool is_const>
inline constexpr bool enable_borrowed_range<spl::graphics::image_range<is_row, is_const>> = true;
} // namespace std::ranges

#endif
