/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:08:45 CEST
 * @license     : MIT
 */

#include "plotter.hpp"

#include <variant>
#include <algorithm>

#include <fmt/format.h>

namespace spl
{

auto plotter::generate_texture() const
    -> sf::Texture
{
    constexpr auto number_of_labels_x = 10;
    constexpr auto number_of_labels_y = 10;

    // find min and max on x and y
    double global_x_min = std::numeric_limits<double>::max();
    double global_x_max = std::numeric_limits<double>::min();
    double global_y_min = std::numeric_limits<double>::max();
    double global_y_max = std::numeric_limits<double>::min();
    for (auto const & [xs, ys, fmt] : _plots) {
        auto const [x_min, x_max] = std::minmax_element(xs.begin(), xs.end());
        auto const [y_min, y_max] = std::minmax_element(ys.begin(), ys.end());
        global_x_min = std::min(global_x_min, *x_min);
        global_x_max = std::max(global_x_max, *x_max);
        global_y_min = std::min(global_y_min, *y_min);
        global_y_max = std::max(global_y_max, *y_max);
    }

    auto x_min_max = std::pair{global_x_min, global_x_max};
    auto y_min_max = std::pair{global_y_min, global_y_max};

    // remap every point in the range [x_min, x_max]x[y_min, y_max]
    auto const primitives = build_primitives(x_min_max, y_min_max);

    // generate axes and labels
    sf::Font font;  // SFML SUCKS
    font.loadFromFile("arial.ttf");
    auto const [axes, labels] = generate_labels(x_min_max, y_min_max, number_of_labels_x, number_of_labels_y, font);

    // generate the texture
    auto texture = sf::RenderTexture{};
    texture.create(_width, _height);
    texture.clear(sf::Color::White);

    // draw axes
    texture.draw(axes);

    // draw graphics
    for (auto const & plot : primitives) {
        std::visit([&texture](auto const & obj) {
            texture.draw(obj);
        }, plot);
    }

    // draw labels
    for (auto const & l : labels) {
        texture.draw(l);
    }

    texture.display();  // if we do not do this, the texture is flipped because SFML.
    return texture.getTexture();
}

auto plotter::get_texture() const
    -> sf::Texture const &
{
    if (not _cached_texture.has_value()) {
        _cached_texture = generate_texture();
    }
    return *_cached_texture;
}

template <typename Pair>
auto plotter::generate_labels(
    Pair const x_min_max, Pair const y_min_max,
    int const number_of_labels_x, int const number_of_labels_y, sf::Font const & font
) const
    -> std::pair<sf::VertexArray, std::vector<sf::Text>>
{
    // configure axes
    sf::VertexArray axes;
    axes.setPrimitiveType(sf::Lines);
    auto const [x_min, x_max] = x_min_max;
    auto const [y_min, y_max] = y_min_max;

    auto const x = plotter::rescale(x_min, x_max, _width, 0.);
    auto const y = plotter::rescale(y_min, y_max, _height, 0.);
    // asse x
    axes.append({sf::Vector2f{x + 0.f,           _border + 0.f}, sf::Color::Black});
    axes.append({sf::Vector2f{x + 0.f, _height - _border + 0.f}, sf::Color::Black});
    // asse y
    axes.append({sf::Vector2f{         _border + 0.f, y + 0.f}, sf::Color::Black});
    axes.append({sf::Vector2f{_width - _border + 0.f, y + 0.f}, sf::Color::Black});

    // configure labels
    std::vector<sf::Text> labels; labels.reserve(number_of_labels_x + number_of_labels_y);

    sf::Text label;
    label.setFont(font);
    label.setFillColor(sf::Color::Black);
    label.setCharacterSize(10);

    auto const width_step = (_width - 2. * _border) / number_of_labels_x;
    auto const x_step     = (x_max - x_min)       / number_of_labels_x;
    for (auto n{0}; n <= number_of_labels_x; ++n)
    {
        auto const x_px = _border + width_step * n;
        auto const x_fn = x_min + x_step * n;
        axes.append({sf::Vector2f(x_px, y + 4), sf::Color::Black});
        axes.append({sf::Vector2f(x_px, y - 4), sf::Color::Black});
        label.setString(fmt::format("{:.2}", x_fn));
        label.setPosition(x_px, y);
        labels.push_back(label);
    }

    auto const height_step = (_height - 2 * _border) / number_of_labels_y;
    auto const y_step      = (y_max - y_min)       / number_of_labels_y;
    for (auto m{0}; m <= number_of_labels_y; ++m)
    {
        auto const y_px = _border + height_step * m;
        auto const y_fn = - (y_min + y_step * m);
        axes.append({sf::Vector2f(x + 4, y_px), sf::Color::Black});
        axes.append({sf::Vector2f(x - 4, y_px), sf::Color::Black});
        if (m != number_of_labels_y / 2) {
            label.setString(fmt::format("{:.2}", y_fn));
            label.setPosition(x, y_px);
            labels.push_back(label);
        }
    }

    return {std::move(axes), std::move(labels)};
}

void plotter::show() const
{
    sf::RenderWindow window(sf::VideoMode(_width, _height), "spl");

    auto const texture = get_texture();
    auto final_plot = sf::Sprite{texture};
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::S) {
                    save_canvas("plot.bmp"); // possible types: bmp, tga, png, jpg
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(final_plot);
        window.display();
    }
}

auto plotter::parse_format(std::string_view const format)
    -> format_result
{
    auto res = format_result{};
    for (auto it = format.cbegin(), end = format.cend(); it != end; ++it) {
        switch (*it) {
        // Color
        case 'B':
        case 'k':
            res.color = sf::Color::Black;
            break;
        case 'b':
            res.color = sf::Color::Blue;
            break;
        case 'g':
            res.color = sf::Color::Green;
            break;
        case 'r':
            res.color = sf::Color::Red;
            break;
        case 'y':
            res.color = sf::Color::Yellow;
            break;
        case 'm':
            res.color = sf::Color::Magenta;
            break;
        // Primitive
        case '.':
            res.figure_code = 0;
            break;
        case 'o':
            res.figure_code = 1;
            break;
        case 'O':
            res.figure_code = 2;
            break;
        case 't':
            res.figure_code = 3;
            break;
        case 's':
            res.figure_code = 4;
            break;
        case 'p':
            res.figure_code = 5;
            break;
        case 'e':
            res.figure_code = 6;
            break;
        case 'T':
            res.figure_code = 13;
            break;
        // case '^', '+', '*', 'x', ...: < 0
        // Line style
        case '-':
            if (auto const next = std::next(it); next != end) {
                if (*next == '-') {
                    res.line_type = line::dashed;
                } else if (*next == '.') {
                    res.line_type = line::dash_dot;
                } else {
                    res.line_type = line::solid;
                }
            }
            break;
        case ':':
            res.line_type = line::dotted;
            break;
        case ' ':
            res.line_type = line::no_line;
            break;
        // Error
        default:
            throw format_error{};
        }
    }

    return res;
}

} // namespace spl
