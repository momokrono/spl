#include <compare>

namespace spl::graphics
{
struct rgba;
class image;

using iterator = std::vector<rgba>::iterator;

template<bool is_row>
struct iter_step;

template <>
struct iter_step<true>
{
    iter_step(int const) {}
};

template <>
struct iter_step<false>
{
    iter_step(int const s) : step{s} {}
    std::ptrdiff_t step;
};


template <bool is_row>
class row_col_iter : public iter_step<is_row>
{
public:
    using iterator_category = iterator::iterator_category;
    using difference_type = iterator::difference_type;
    using value_type = iterator::value_type;
    using reference = iterator::reference;
    using pointer = iterator::pointer;

    row_col_iter() : iter_step<is_row>{1} {}
    row_col_iter(int const s) : iter_step<is_row>{s} {}

    auto operator+=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it+=n;
        } else {
            _it+=n*step;
        }
        return *this;
    }

    auto operator-=(difference_type const n)
        -> row_col_iter&
    {
        if constexpr (is_row) {
            _it+=n;
        } else {
            _it+=n*step;
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
            _it+=step;
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
            _it-=step;
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

    auto operator[](difference_type const n) const
        -> const reference
    {
        return _it[n];
    }

    auto operator<=>(const row_col_iter&) const = default;

    auto operator*()
        -> reference
    {
        return *_it;
    }

    auto operator*() const
        -> const reference
    {
        return *_it;
    }

    auto operator->()
        -> pointer
    {
        return std::addressof(*_it);
    }

    auto operator->() const
        -> const pointer
    {
        return std::addressof(*_it);
    }

private:
    iterator _it;
};

template <bool is_row>
class image_range
{
    friend class image;
    using iterator = row_col_iter<is_row>;
    iterator begin() const { return _begin; };
    iterator end() const { return _begin+_count; };

private:
    image_range(iterator const b, int const c) : _begin{b}, _count{c} {}
    iterator _begin;
    int _count;
};

class o {};
} // namespace spl::graphics
