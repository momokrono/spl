/**
 * @author      : rbrugo, momokrono
 * @file        : renderer
 * @created     : Tuesday Nov 10, 2020 10:54:18 CET
 * @license     : MIT
 * */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "spl/drawable.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

class collection
{
    struct object_t;

    std::vector<object_t> _buffer;

public:
    void render_on(graphics::viewport img) const noexcept;
    template <drawable T>
    collection & push(T obj) noexcept;
    void clear() noexcept { _buffer.clear(); }
    void reserve(std::size_t n) { _buffer.reserve(n); }
};

struct collection::object_t
{
    template <drawable T>
    explicit object_t(T obj) : _self{std::make_shared<model<T>>(std::move(obj))} {}

    struct concept_t
    {
        virtual ~concept_t() = default;
        virtual void render_on(graphics::viewport) const noexcept = 0;
    };

    template <drawable T>
    struct model final : concept_t
    {
        explicit model(T obj) : _data{std::move(obj)} {}
        void render_on(graphics::viewport img) const noexcept final
        {
            if constexpr (spl::detail::has_render_on_member_function<T>) {
                _data.render_on(img);
            } else {
                _data(img);
            }
        }
        T _data;
    };

    void render_on(graphics::viewport img) const noexcept
    {
        _self->render_on(img);
    }

    std::shared_ptr<concept_t> _self;
};

inline
void collection::render_on(graphics::viewport img) const noexcept
{
    for (auto const & obj : _buffer) {
        obj.render_on(img);
    }
}

template <drawable T>
inline
auto collection::push(T obj) noexcept -> collection &
{
    _buffer.emplace_back(std::move(obj));
    return *this;
}

} // namespace spl::graphics

#endif /* RENDERER_HPP */

