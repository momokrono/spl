/**
 * @author      : rbrugo, momokrono
 * @file        : drawable
 * @created     : Thursday Jun 11, 2020 22:22:44 CEST
 * @license     : MIT
 * */

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

namespace spl
{

namespace graphics
{
class image;
} // namespace graphics

namespace detail
{
// FIXME `img` must be passed by (non-const) reference
template <typename T>
concept has_render_on_member_function = requires (T & t, graphics::image & img) { { t.render_on(img) }; };
template <typename T>
concept callable_with_image = requires (T & t, graphics::image & img) { { t(img) }; };
} // namespace detail

template <typename T>
concept drawable = detail::has_render_on_member_function<T> or detail::callable_with_image<T>;

} // namespace spl

#endif /* DRAWABLE_HPP */

