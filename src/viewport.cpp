/**
 * @author      : rbrugo, momokrono
 * @file        : basic_viewport
 * @created     : Sunday Mar 21, 2021 22:32:24 CET
 * @license     : MIT
 */

#include "spl/viewport.hpp"

namespace spl::graphics
{

template <bool Const>
auto basic_viewport<Const>::pixel(size_t const x, size_t const y) -> reference
{
    if (x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, width(), height()};
    }
    return _base->pixel(_x + x, _y + y);
}

template <bool Const>
auto basic_viewport<Const>::pixel(size_t const x, size_t const y) const -> const_reference
{
    if (x >= width() or y >= height()) {
        throw spl::out_of_range{x, y, width(), height()};
    }
    return _base->pixel(_x + x, _y + y);
}

template <bool Const>
auto basic_viewport<Const>::pixel_noexcept(size_t const x, size_t const y) noexcept -> reference
{
    if (x >= width() or y >= height()) {
        return _base->_garbage_pixel;
    }
    return _base->pixel_noexcept(_x + x, _y + y);
}

template <bool Const>
auto basic_viewport<Const>::pixel_noexcept(size_t const x, size_t const y) const noexcept -> const_reference
{
    if (x >= width() or y >= height()) {
        return _base->_garbage_pixel;
    }
    return _base->pixel_noexcept(_x + x, _y + y);
}

template <bool Const>
auto basic_viewport<Const>::row(size_t const y) -> row_view
{
    auto it = _base->get_pixel_iterator(_x, _y + y);
    return {it, width()};
}

template <bool Const>
auto basic_viewport<Const>::row(size_t const y) const -> const_row_view
{
    auto it = std::as_const(*_base).get_pixel_iterator(_x, _y + y);
    return {it, width()};
}

template <bool Const>
auto basic_viewport<Const>::column(size_t const x) -> column_view
{
    auto it = _base->get_pixel_iterator(_x + x, _y);
    return {it, width()};
}

template <bool Const>
auto basic_viewport<Const>::column(size_t const x) const -> const_column_view
{
    auto it = std::as_const(*_base).get_pixel_iterator(_x + x, _y);
    static_assert(std::same_as<decltype(it), std::vector<rgba>::const_iterator>);
    return {it, width()};
}

template <bool Const>
auto basic_viewport<Const>::rows() -> row_range
{
    return {row(_x), height()};
}

template <bool Const>
auto basic_viewport<Const>::rows() const -> const_row_range
{
    return {row(_x), height()};
}

template <bool Const>
auto basic_viewport<Const>::columns() -> column_range
{
    return {column(_y), width()};
}

template <bool Const>
auto basic_viewport<Const>::columns() const -> const_column_range
{
    return {column(_y), width()};
}

template <>
auto basic_viewport<false>::fill(rgba const c) & noexcept -> basic_viewport &
{
    for (auto i : std::views::iota(height())) {
        std::ranges::fill(row(i), c);
    }
    return *this;
}

template <>
auto basic_viewport<false>::fill(rgba const c) && noexcept -> basic_viewport
{
    for (auto i : std::views::iota(height())) {
        std::ranges::fill(row(i), c);
    }
    return *this;
}

template class basic_viewport<true>;
template class basic_viewport<false>;

} // namespace spl::graphics
