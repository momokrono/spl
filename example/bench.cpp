/**
 * @author      : rbrugo. momokrono
 * @file        : bench
 * @created     : Friday Jun 12, 2020 14:13:07 CEST
 * @license     : MIT
 */

#include <fmt/format.h>
#include <ranges>

#include "spl/image.hpp"
#include "spl/primitive.hpp"

int main(int argc, [[maybe_unused]] char * argv[]) try
{
    auto image = spl::graphics::image(600,400);
    image.fill(spl::graphics::color::white);
    fmt::print(stdout, "{}", argc == 1 ? "new\n" : "old\n");
//    for (int j = 0; j <= 66; ++j) {
//        auto line1 = spl::graphics::line{{0,   200}, {599, 200 + j * 3 }, spl::graphics::color::green};
//        auto line2 = spl::graphics::line{{599, 200}, {  0, 200 + j * 3 }, spl::graphics::color::blue};
//        auto line3 = spl::graphics::line{{0,   200}, {599, 200 - j * 3 }, spl::graphics::color::green};
//        auto line4 = spl::graphics::line{{599, 200}, {  0, 200 - j * 3 }, spl::graphics::color::blue};
//        if (argc == 1) {
//            line1.draw_antialiased_parametric(image);
//            line2.draw_antialiased_parametric(image);
//            line3.draw_antialiased_parametric(image);
//            line4.draw_antialiased_parametric(image);
//        } else {
//            line1.draw_antialiased(image);
//            line2.draw_antialiased(image);
//            line3.draw_antialiased(image);
//            line4.draw_antialiased(image);
//        }
//    }
    auto c1 = spl::graphics::vertex{100, 200};
    auto c2 = spl::graphics::vertex{200, 50};
    auto c3 = spl::graphics::vertex{400, 300};
    auto c4 = spl::graphics::vertex{500, 200};
    auto bezier = spl::graphics::bezier{c1, c2, c3, c4, 5, 40, spl::graphics::color::red};
    auto line1 = spl::graphics::line(c1, c2, spl::graphics::color::cyan);
    auto line2 = spl::graphics::line(c3, c4, spl::graphics::color::cyan);
    line1.render_on(image);
    line2.render_on(image);
    image.draw(spl::graphics::circle(c1, 4).fill_color(spl::graphics::color::blue));
    image.draw(spl::graphics::circle(c2, 4).fill_color(spl::graphics::color::blue));
    image.draw(spl::graphics::circle(c3, 4).fill_color(spl::graphics::color::blue));
    image.draw(spl::graphics::circle(c4, 4).fill_color(spl::graphics::color::blue));
//    bezier.render_on(image);
    image.draw(bezier);
    // l.render_on(image);
    if (not image.save_to_file("bench.png")) { fmt::print("NON VA\n"); }
} catch (std::exception & e)
{
    fmt::print(stderr, "exception caught: {}\n", e.what());
}


