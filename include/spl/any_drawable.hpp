/**
 * @author      : rbrugo, momokrono
 * @file        : any_drawable
 * @created     : Sunday May 16, 2021 20:08:51 CEST
 * @description : implements `any_drawable`, a type-erasing wrapper to store `drawable`s
 * */

#ifndef ANY_DRAWABLE_HPP
#define ANY_DRAWABLE_HPP

#include <memory>
#include "spl/drawable.hpp"
#include "spl/viewport.hpp"

namespace spl::graphics
{

/** A polymorphic type to save any drawable type
 *
 *  This is a type-erasing, polymorphic wrapper that can contain any object satisfying the
 *  requirements of `drawable`. It can be used to store `drawable`s whose type is not known
 *  compile-time.
 * */
struct any_drawable
{
    template <drawable T>
    explicit any_drawable(T obj) : _self{std::make_shared<model<T>>(std::move(obj))} {}

    any_drawable(any_drawable const &) = default;
    any_drawable(any_drawable &&) = default;
    any_drawable & operator=(any_drawable const &) = default;
    any_drawable & operator=(any_drawable &&) = default;

    void render_on(graphics::viewport img) const noexcept
    {
        if (_self) {
            _self->render_on(img);
        }
    }

private:
    struct concept_t
    {
        virtual ~concept_t() noexcept = default;
        virtual void render_on(graphics::viewport) const = 0;
    };

    template <drawable T>
    struct model final : concept_t
    {
        explicit model(T obj) : _data{std::move(obj)} {}
        void render_on(graphics::viewport img) const final
        {
            if constexpr (spl::detail::has_render_on_member_function<T>) {
                _data.render_on(img);
            } else {
                _data(img);
            }
        }
        T _data;
    };

    std::shared_ptr<concept_t> _self;
};


} // namespace spl::graphics

#endif /* ANY_DRAWABLE_HPP */

