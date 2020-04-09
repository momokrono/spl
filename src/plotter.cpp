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
        sf::RenderWindow window(sf::VideoMode(_width, _height), "titolo");
        sf::VertexArray axes;

        sf::VertexArray function{sf::LinesStrip};   // TODO: aggiungere la possibilità di cambiare primitiva
        axes.setPrimitiveType(sf::Lines);           // per poter fare linee tratteggiate, cerchi, tratto punto, ...
                                                    // Lines, LinesStrip, Points come primitive di un vertex array,
                                                    // altrimenti si va di vettore di cerchi, quadrati, stelline, ...
        auto points = get_plot_points();
        for (auto [x, y] : points)
        {
            sf::Vertex point;
            point.position = sf::Vector2f(x, _height-y);
            point.color = sf::Color::Blue;
            function.append(point);
        }

        // TODO: abbellire sta roba, aggiungere le misure sugli assi
        std::vector<sf::Text> labels;
        sf::Font font;
        font.loadFromFile("arial.ttf"); // magari usare un font di sistema
        sf::Text label;
        label.setFont(font);
        label.setFillColor(sf::Color::Black);
        label.setCharacterSize(10);

        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_max] = std::minmax_element(_ys.begin(), _ys.end());
        auto const x = plotter::rescale(*x_min, *x_max, _width, 0.);
        auto const y = plotter::rescale(*y_min, *y_max, _height, 0.);
        sf::Vertex vert;
        vert.color = sf::Color::Black;
        // asse x
        vert.position = sf::Vector2f(x,_border);
        axes.append(vert);
        vert.position = sf::Vector2f(x,_height-_border);
        axes.append(vert);
        // asse y
        vert.position = sf::Vector2f(_border,y);
        axes.append(vert);
        vert.position = sf::Vector2f(_width-_border,y);
        axes.append(vert);
        // qui ci sono i segnetti sugli assi, 10 per asse
        // TODO: numero modificabile? diverso per x e y? aggiungere le misure
        for (auto n{0}; n <= 10; ++n)
        {
            auto i = _border + (_width-2*_border)*n/10;
            auto t = *x_min + (*x_max-*x_min)*n/10;
            vert.position = sf::Vector2f(i, y+4);
            axes.append(vert);
            vert.position = sf::Vector2f(i, y-4);
            axes.append(vert);
            label.setString(fmt::format("{:.2}", t));
            label.setPosition(sf::Vector2f(i, y));
            labels.push_back(label);
        }
        for (auto m{0};  m<= 10; ++m)
        {
            auto j = _border + (_height-2*_border)*m/10;
            auto t = - (*y_min + (*y_max-*y_min)*m/10);
            vert.position = sf::Vector2f(x+4, j);
            axes.append(vert);
            vert.position = sf::Vector2f(x-4, j);
            axes.append(vert);
            label.setString(fmt::format("{:.2}", t));
            label.setPosition(sf::Vector2f(x, j));
            labels.push_back(label);
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
                        auto plot = std::pair<sf::VertexArray, sf::VertexArray>(function, axes);
                        save_canvas(plot, labels);
                    }
                }
            }

            window.clear(sf::Color::White);
            window.draw(axes);
            window.draw(function);
            for (auto & l : labels)
            {
                window.draw(l);
            }
            window.display();
        }
    }
} // namespace spl
