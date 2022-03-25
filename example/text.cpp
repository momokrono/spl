/**
 * @author      : rbrugo, momokrono
 * @file        : text
 * @created     : Friday Mar 25, 2022 15:27:21 CET
 * @description : 
 */

#include <spl/text.hpp>
#include <spl/image.hpp>

int main(int argc, char * argv[])
{
    if (argc == 1) {
        fmt::print("Usage: {} {} [{}...]\n", argv[0], "font-name", "other-font-names");
        return 0;
    }

    auto img = spl::graphics::image{800, 600, spl::graphics::color::white};
    auto font1 = spl::graphics::font{argv[1], 95};
    auto font2 = spl::graphics::font{argv[std::min(2, argc - 1)], 20};
    auto text1 = spl::graphics::text{
        spl::graphics::vertex{153, 200}, "S@$s° Gìà£l0", argv[std::min(3, argc - 1)], 40,
        spl::graphics::color::red
    };
    auto text2 = spl::graphics::text{
        spl::graphics::vertex{50, 400}, "r0ùνδ€D κow 🌈", font1, spl::graphics::color::yellow
    };
    auto text3 = spl::graphics::text{{50, 270}, "abcdefghijklmnopqrstuvwxyz", font2};
    auto text4 = spl::graphics::text{{25, 270 + 60}, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", font2};
    auto text5 = spl::graphics::text{{75, 270 + 120}, "0123456789", font2};
    auto text6 = spl::graphics::text{{50, 110}, "€", font1, spl::graphics::color::red};

    img.draw(text1).draw(text2).draw(text3).draw(text4).draw(text5).draw(text6);

    img.save_to_file("text_output.png");
    return 0;
}
