/**
 * @author      : rbrugo, momokrono
 * @file        : text
 * @created     : Friday Mar 25, 2022 15:27:21 CET
 * @description : 
 */

#include <spl/text.hpp>
#include <spl/image.hpp>

constexpr auto lorem_ipsum = std::array{
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit,",
    "sed do eiusmod tempor incididunt ut labore et dolore",
    "magna aliqua. Ut enim ad minim veniam, quis nostrud",
    "exercitation ullamco laboris nisi ut aliquip ex ea",
    "commodo consequat. Duis aute irure dolor in reprehenderit",
    "in voluptate velit esse cillum dolore eu fugiat nulla",
    "pariatur. Excepteur sint occaecat cupidatat non proident,",
    "sunt in culpa qui officia deserunt mollit anim id est laborum."
};

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
        spl::graphics::vertex{153, 200}, "S@$s° Gìà£l0", font1, 40,
        spl::graphics::color::red
    };
    auto text2 = spl::graphics::text{{50, 270}, "abcdefghijklmnopqrstuvwxyz", font2};
    auto text3 = spl::graphics::text{{25, 270 + 60}, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", font2};
    auto text4 = spl::graphics::text{{75, 270 + 120}, "0123456789", font2};

    img.draw(text1).draw(text2).draw(text3).draw(text4);

    for (auto i = 0; i < lorem_ipsum.size(); ++i) {
        auto text = spl::graphics::text{{20, 20 + i * 20}, lorem_ipsum[i], font2};
        img.draw(text);
    }

    img.save_to_file("text_output.png");
    return 0;
}
