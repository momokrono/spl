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
// class image;
template <bool B>
class basic_viewport;
} // namespace graphics

namespace detail
{
// FIXME `img` must be passed by (non-const) reference
template <typename T>
concept has_render_on_member_function = requires (T & t, graphics::basic_viewport<false> img) {
    { t.render_on(img) };
};
template <typename T>
concept callable_with_image = requires (T & t, graphics::basic_viewport<false> img) { { t(img) }; };
} // namespace detail

/**
  A type satisfies the concept of `drawable` if either it has a `render_on` method which accepts
  a `spl::graphics::viewport`, or if it is callable with an argument of type `spl::graphics::viewport`.

  \rst
  **Example**::

    struct draw_pixel
    {
        int x = 0;
        int y = 0;
        spl::graphics::rgba color = spl::graphics::colors::black;

        void render_on(spl::graphics::viewport v) const
        {
            v.pixel(x, y) = color;
        }
    };

    auto draw_pixel_2 = [](spl::graphics::viewport v) {
        v.pixel(4, 5) = spl::graphics::color::red;
    };

    static_assert(spl::drawable<draw_pixel>);
    static_assert(spl::drawable<decltype(draw_pixel_2)>);

  \endrst
 * */
template <typename T>
concept drawable = detail::has_render_on_member_function<T> or detail::callable_with_image<T>;

} // namespace spl

#endif /* DRAWABLE_HPP */

