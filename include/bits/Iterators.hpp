#ifndef _ITERATORS_HPP_
#define _ITERATORS_HPP_

#include <compare>
#include <vector>
#include <ranges>

#include "Graphics/rgba.hpp"

namespace spl::graphics
{
class image;

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
    iter_step(size_t const s = 1) : step{s} {}
    std::size_t step;
};


template <bool is_row>
class row_col_iter : iter_step<is_row>
{
public:
    using iterator = std::vector<rgba>::iterator;
    using iterator_category = iterator::iterator_category;
    using difference_type = iterator::difference_type;
    using value_type = iterator::value_type;
    using reference = iterator::reference;
    using const_reference = value_type;
    using pointer = iterator::pointer;

    row_col_iter() : iter_step<is_row>{1} {}
    row_col_iter(iterator i) : _it{i} {}
    row_col_iter(size_t const s) : iter_step<is_row>{s} {}
    row_col_iter(iterator i, size_t const s) : iter_step<is_row>{s}, _it{i} {}

    auto operator+=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it+=n;
        } else {
            _it+=n*iter_step<is_row>::step;
        }
        return *this;
    }

    auto operator-=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it+=n;
        } else {
            _it+=n*iter_step<is_row>::step;
        }
        return *this;
    }

    auto operator+(difference_type const n) const
        -> row_col_iter
    { 
        auto r = *this;
        r += n;
        return r;
    }

    auto operator-(difference_type const n) const
        -> row_col_iter
    { 
        auto r = *this;
        r -= n;
        return r;
    }

    auto operator-(row_col_iter const a) const
        -> difference_type
    {
        return _it - a._it;
    }

    auto operator++()
        -> row_col_iter&
    {
        if constexpr (is_row) {
            ++_it;
        } else {
            _it+=iter_step<is_row>::step;
        }
        return *this;
    }

    auto operator++(int)
        -> row_col_iter
    {
        auto r = *this;
        ++*this;
        return r;
    }

    auto operator--()
        -> row_col_iter&
    {
        if constexpr (is_row) {
            --_it;
        } else {
            _it-=iter_step<is_row>::step;
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

    auto operator[](difference_type const n)
        -> reference
    {
        return _it[n];
    }

    auto operator[](difference_type const n) const -> const_reference
    { return _it[n]; }

    /* auto operator== (row_col_iter const other) const noexcept { return _it == other._it; } */
    /* auto operator<=>(row_col_iter other) const noexcept { return _it <=> other._it; } */
    friend auto operator== (row_col_iter<is_row> const lhs, row_col_iter<is_row> const rhs) noexcept
    { return lhs._it == rhs._it; }
    friend auto operator<=>(row_col_iter<is_row> const lhs, row_col_iter<is_row> const rhs) noexcept
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

template <bool is_row>
class image_range : std::ranges::view_base
{
public:
    using value_type = rgba;
    using iterator = row_col_iter<is_row>;
    using const_iterator = iterator;
    using reference = iterator::reference;
    using const_reference = iterator::reference;

private:
    friend class image;
    iterator _begin;
    size_t _count = 0;

public:
    image_range() = default;
    image_range(iterator const b, size_t const c) : _begin{b}, _count{c} {}
    iterator begin()  const { return _begin; };
    iterator end()    const { return _begin + _count; };
    iterator cbegin() const { return _begin; };
    iterator cend()   const { return _begin + _count; };

    friend bool operator==(image_range const, image_range const) = default;
};

} // namespace spl::graphics

namespace std::ranges
{
template <bool is_row>
inline constexpr bool enable_view<spl::graphics::image_range<is_row>> = true;
template <bool is_row>
inline constexpr bool enable_borrowed_range<spl::graphics::image_range<is_row>> = true;
} // namespace std::ranges
#endif
