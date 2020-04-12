/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:08:45 CEST
 * @license     : MIT
 */

#include "plotter.hpp"

#include <algorithm>

#include <fmt/format.h>

namespace spl
{
    void plotter::show() const
    {
        constexpr auto number_of_labels_x = 10;
        constexpr auto number_of_labels_y = 10;
        sf::RenderWindow window(sf::VideoMode(_width, _height), "titolo");
        sf::VertexArray axes;

        sf::VertexArray function{sf::LinesStrip};   // TODO: aggiungere la possibilità di cambiare primitiva
        axes.setPrimitiveType(sf::Lines);           // per poter fare linee tratteggiate, cerchi, tratto punto, ...
                                                    // Lines, LinesStrip, Points come primitive di un vertex array,
                                                    // altrimenti si va di vettore di cerchi, quadrati, stelline, ...
        auto points = get_plot_points();
        for (auto const [x, y] : points)
        {
            auto const point = sf::Vertex{{x + 0.f, _height - y + 0.f}, sf::Color::Blue};
            function.append(point);
            // function.append({{x + 0.f, _height - y + 0.f}, sf::Color::Blue}); // or this
        }

        // TODO: abbellire sta roba, aggiungere le misure sugli assi
        std::vector<sf::Text> labels; labels.reserve(number_of_labels_x + number_of_labels_y);

        sf::Font font;
        font.loadFromFile("arial.ttf"); // magari usare un font di sistema

        sf::Text label;
        label.setFont(font);
        label.setFillColor(sf::Color::Black);
        label.setCharacterSize(10);

        // draw axis
        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_max] = std::minmax_element(_ys.begin(), _ys.end());
        auto const x = plotter::rescale(*x_min, *x_max, _width, 0.);
        auto const y = plotter::rescale(*y_min, *y_max, _height, 0.);
        // asse x
        axes.append({sf::Vector2f{x + 0.f,           _border + 0.f}, sf::Color::Black});
        axes.append({sf::Vector2f{x + 0.f, _height - _border + 0.f}, sf::Color::Black});
        // asse y
        axes.append({sf::Vector2f{         _border + 0.f, y + 0.f}, sf::Color::Black});
        axes.append({sf::Vector2f{_width - _border + 0.f, y + 0.f}, sf::Color::Black});

        // qui ci sono i segnetti sugli assi
        // TODO: Aggiungere le misure
        sf::Vertex vert;
        vert.color = sf::Color::Black;
        auto const width_step = (_width - 2. * _border) / number_of_labels_x;
        auto const x_step     = (*x_max - *x_min)       / number_of_labels_x;
        for (auto n{0}; n <= number_of_labels_x; ++n)
        {
            auto const x_px = _border + width_step * n;
            auto const x_fn = *x_min + x_step * n;
            vert.position = sf::Vector2f(x_px, y + 4);
            axes.append(vert);
            vert.position = sf::Vector2f(x_px, y - 4);
            axes.append(vert);
            // if (n == 5)
            //     continue; // bruttissimo
            label.setString(fmt::format("{:.2}", x_fn));
            label.setPosition(x_px, y);
            labels.push_back(label);
        }

        auto const height_step = (_height - 2 * _border) / number_of_labels_y;
        auto const y_step      = (*y_max - *y_min)       / number_of_labels_y;
        for (auto m{0}; m <= number_of_labels_y; ++m)
        {
            auto const y_px = _border + height_step * m;
            auto const y_fn = - (*y_min + y_step * m);
            vert.position = sf::Vector2f(x + 4, y_px);
            axes.append(vert);
            vert.position = sf::Vector2f(x - 4, y_px);
            axes.append(vert);
            if (m != number_of_labels_y / 2) {
                label.setString(fmt::format("{:.2}", y_fn));
                label.setPosition(x, y_px);
                labels.push_back(label);
            }
        }


        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        window.close();
                    }
                    if (event.key.code == sf::Keyboard::S)
                    {
                        // auto plot = std::pair<sf::VertexArray, sf::VertexArray>(function, axes);
                        save_canvas({function, axes}, labels, "plot.bmp"); // possible types: bmp, tga, png, jpg
                    }
                }
            }

            window.clear(sf::Color::White);
            window.draw(axes);
            window.draw(function);
            for (auto const & l : labels)
            {
                window.draw(l);
            }
            window.display();
        }
    }
} // namespace spl
