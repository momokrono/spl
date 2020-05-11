/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:08:45 CEST
 * @license     : MIT
 */

#include "plotter.hpp"

#include <variant>
#include <algorithm>

#include <range/v3/algorithm.hpp>
#include <range/v3/view/take_while.hpp>

#include <fmt/format.h>

namespace spl
{

auto plotter::generate_texture() const
    -> sf::Texture
{
    constexpr auto number_of_labels_x = 10;
    constexpr auto number_of_labels_y = 10;

    // find min and max on x and y
    double x_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::min();
    double y_min = std::numeric_limits<double>::max();
    double y_max = std::numeric_limits<double>::min();
    for (auto const & [xs, ys, fmt] : _plots) {
        auto const [local_x_min, local_x_max] = std::minmax_element(xs.begin(), xs.end());
        auto const [local_y_min, local_y_max] = std::minmax_element(ys.begin(), ys.end());
        x_min = std::min(x_min, *local_x_min);
        x_max = std::max(x_max, *local_x_max);
        y_min = std::min(y_min, *local_y_min);
        y_max = std::max(y_max, *local_y_max);
    }

    auto const x_min_max = std::pair{x_min, x_max};
    auto const y_min_max = std::pair{y_min, y_max};

    // remap every point in the range [x_min, x_max]x[y_min, y_max]
    auto const primitives = build_primitives(x_min_max, y_min_max);

    // generate axes and labels
    auto font = sf::Font{};
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

struct axes_config
{
    double min;
    double max;
    double step;
    double order;
};

auto generate_axes_config(double const min, double const max)
    -> axes_config
{
    namespace rvw = ranges::views;
    constexpr auto steps_sizes = std::array{5., 2.5, 2., 1.5, 1.};
    auto const order = std::pow(10, std::floor(std::log10(max - min)) - 1.);
    auto const min_scaled  = std::floor(min / order);
    auto const max_scaled  = std::floor(max / order);

    constexpr auto n_steps = [](auto min, auto max, auto step) {
        return std::floor((max - min) / step);
    };

    auto const factor = n_steps(min_scaled, max_scaled, 5) > 9 ? 10 : 1;

    auto number_of_steps = steps_sizes
                         | rvw::transform([factor](auto n) { return n * factor; })
                         | rvw::transform([=, &n_steps](auto n) { return n_steps(min_scaled, max_scaled, n); })
                         | rvw::take_while([](auto n) { return n <= 10; })
                         ;

    auto const max_num_of_step_it = ranges::max_element(number_of_steps).base();
    auto const best_step          = *(max_num_of_step_it.base());
    auto const actual_min_scaled  = (min_scaled - std::fmod(min_scaled, best_step));

    return {actual_min_scaled, max_scaled, best_step, order};
}

template <typename Pair>
auto plotter::generate_labels(
    Pair const x_min_max, Pair const y_min_max,
    int const number_of_labels_x, int const number_of_labels_y, sf::Font const & font
) const
    -> std::pair<sf::VertexArray, std::vector<sf::Text>>
{
    namespace rvw = ranges::views;

    // configure axes
    auto axes = sf::VertexArray{};
    axes.setPrimitiveType(sf::Lines);
    auto const [x_min, x_max] = x_min_max;
    auto const [y_min, y_max] = y_min_max;

    auto const x = plotter::rescale(x_min, x_max, _width, 0.);
    auto const y = plotter::rescale(y_min, y_max, _height, 0.);
    // asse y
    axes.append({sf::Vector2f{x + 0.f,           _border + 0.f}, sf::Color::Black});
    axes.append({sf::Vector2f{x + 0.f, _height - _border + 0.f}, sf::Color::Black});
    // asse x
    axes.append({sf::Vector2f{         _border + 0.f, _height - y + 0.f}, sf::Color::Black});
    axes.append({sf::Vector2f{_width - _border + 0.f, _height - y + 0.f}, sf::Color::Black});

    // calculates the number of labels along the axes
    // ...
    //                  0.5
    //    1 1.5  2  2.5   5
    //   10  15 20   25  50
    //  100 150 200 250
    // ...
    //  - order of magnitude
    //  - decrease it by 1
    //  - divide
    //
    // Examples:
    //
    //  4e21 -> step of 1e20
    //  [0, 100'000] -> step (order of magnitude) 10'000
    //  "rescale" the interval according the order of magnitude of the step: [0, 10]
    //  try {1, 1.5, 2, 2.5, 5} in decreasing order, take the number wich has the highest number of
    //      labels (5<=n<=9)
    //
    //  [-1e6, 1e6] -> 1e5
    //  -> [-10, 10]
    //  - 5: -10 -5 0 5 10      // 5
    //  - 2.5: -10 -7.5 -5 -2.5 0 2.5 5 7.5 10 // 9
    //
    //
    //  2n + 1
    //  2(n+1) + 1
    //
    //
    //  [-1.553, 5.68] -> // 0.1
    //  -> [-15.53, 56.8] -> [-15, 56] (floor)
    //  - 5: -15 -10 -5 0 5 10 15 20 25 ... => too big => multiply by 10
    //  - 50: -15 35 no
    //  - 25: -15 10 35 no
    //  - 20: -15 5 25 45 no
    //  - 15: -15 0 15 30 45 OK
    //  - 10: -15 -5 5 15 25 35 45 55 OK
    //  [-15, 56] => (rounding depends on the step) [-15, 55] => [-1.5, 5.5]
    //  step 10
    //  diff = min_x % step
    //  min3 = min_x - diff
    //  -15 % 10 = -5
    //  -15 - -5 = -10

    auto const [axis_min, axis_max, best_step, order]         = generate_axes_config(x_min, x_max);
    auto const [axis_min_y, axis_max_y, best_step_y, order_y] = generate_axes_config(y_min, y_max);

    // configure labels
    auto labels = std::vector<sf::Text>{};
    labels.reserve(number_of_labels_x + number_of_labels_y);

    auto label = sf::Text{};
    label.setFont(font);
    label.setFillColor(sf::Color::Black);
    label.setCharacterSize(10);

    for (auto cursor = axis_min, x_cap = axis_max + best_step / 2; cursor < x_cap; cursor += best_step) {
        auto const x_fn = cursor * order;
        auto const x_px = rescale(x_min, x_max, _width, x_fn);
        axes.append({sf::Vector2f(x_px, _height - y + 4), sf::Color::Black});
        axes.append({sf::Vector2f(x_px, _height - y - 4), sf::Color::Black});
        label.setString(fmt::format("{:.2f}", x_fn));
        label.setPosition(x_px, _height - y);
        labels.push_back(label);
    }

    for (auto cursor = axis_min_y, y_cap = axis_max_y + best_step_y / 2; cursor < y_cap; cursor += best_step_y) {
        auto const y_fn = cursor * order_y;
        auto const y_px = rescale(y_min, y_max, _height, y_fn);
        axes.append({sf::Vector2f(x + 4, _height - y_px), sf::Color::Black});
        axes.append({sf::Vector2f(x - 4, _height - y_px), sf::Color::Black});
        label.setString(fmt::format("{:.2f}", y_fn));
        label.setPosition(x, _height - y_px);
        labels.push_back(label);
    }
    return {std::move(axes), std::move(labels)};
}

void plotter::show() const
{
    auto const texture = get_texture();
    auto const final_plot = sf::Sprite{texture};

    auto window = sf::RenderWindow{sf::VideoMode(_width, _height), "spl"};
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                case sf::Keyboard::Q:
                    window.close();
                    break;
                case sf::Keyboard::S:
                    save_plot("plot.bmp");
                    break;
                default:
                    ;
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
