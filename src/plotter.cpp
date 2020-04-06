/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:08:45 CEST
 * @license     : MIT
 */

#include "plotter.hpp"

#include <functional>
#include <iostream>
#include <fstream>

#include <fmt/format.h>

#include <range/v3/view/transform.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/zip.hpp>

namespace spl
{
    auto plotter::save_as_pmm(std::filesystem::path const & destination) const
        -> bool
    {
        namespace rvw = ranges::views;
        auto sink = std::ofstream{destination};
        // TODO: check if file is good
        // assume: _xs.begin() != _xs.end();
        
        auto buffer = std::vector<uint8_t>(_width * _height * 3, 0xFF);
        auto vectors = get_plot_points();

        for (auto const [x, y] : vectors) {
            auto const base_offset = (x + y * _width) * 3;
            buffer.at(base_offset + 0) = 0x00;
            buffer.at(base_offset + 1) = 0x00;
            buffer.at(base_offset + 2) = 0x00;
        }

        // PMM FILE FORMAT
        // 1) 'P3'
        // 2) width height
        // 3) max_scale_factor (255)
        // 4) data... 1 line x pixel
        sink << fmt::format("P3\n{} {}\n255\n", _width, _height);
        for (auto const pixel : buffer | rvw::chunk(3)) {
            sink << fmt::format("{} {} {}\n", pixel[0], pixel[1], pixel[2]);
        }

        return true;
    }

    void plotter::show() const
    {
        namespace rvw = ranges::views;
        // anti aliasing, ma non mi piace l'effetto
        // sf::ContextSettings ctx;
        // ctx.antialiasingLevel=2;
        // sf::RenderWindow window(sf::VideoMode(_width, _height), "titolo", sf::Style::Default, ctx);

        sf::RenderWindow window(sf::VideoMode(_width, _height), "titolo");
        sf::VertexArray axes;

        sf::VertexArray vertexes{sf::LinesStrip};   // TODO: aggiungere la possibilità di cambiare primitiva
        axes.setPrimitiveType(sf::Lines);           // per poter fare linee tratteggiate, cerchi, tratto punto, ...
                                                    // Lines, LinesStrip, Points come primitive di un vertex array,
                                                    // altrimenti si va di vettore di cerchi, quadrati, stelline, ...
        auto points = get_plot_points();
        for (auto [x, y] : points)
        {
            sf::Vertex point;
            point.position = sf::Vector2f(x, _height-y);
            point.color = sf::Color::Blue;
            vertexes.append(point);
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
            label.setString(fmt::format("{:.2}", t)); // TODO: inserire qui il numero su R della mia funzione, non i pixel, nel vettore _xs
            label.setPosition(sf::Vector2f(i, y)); // questo è ok
            labels.push_back(label);
        }
        for (auto m{0};  m<= 10; ++m)
        {
            auto j = _border + (_height-2*_border)*m/10;
            auto t = *y_max + (*y_min-*y_max)*m/10;
            vert.position = sf::Vector2f(x+4, j);
            axes.append(vert);
            vert.position = sf::Vector2f(x-4, j);
            axes.append(vert);
            label.setString(fmt::format("{:.2}", t)); // TODO: come sopra, vettore _ys
            label.setPosition(sf::Vector2f(x, j)); // questo è ok
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
                        sf::RenderTexture texture;
                        texture.create(_width, _height);
                        texture.clear(sf::Color::White);
                        auto reflect = [](sf::Vertex & v){
                                v.position.y = -v.position.y;
                        };
                        auto ax = axes;
                        auto vx = vertexes;
                        for (std::size_t i{0}; i < vx.getVertexCount(); ++i)
                        {
                            reflect(vx[i]);
                        }
                        for (std::size_t i{0}; i < ax.getVertexCount(); ++i)
                        {
                            reflect(ax[i]);
                        }
                        // auto axs = axes | rvw::transform([](auto & v){reflect(v);});
                        texture.draw(ax);
                        texture.draw(vx);
                        for (auto & l : labels)
                        {
                            texture.draw(l);
                        }
                        texture.getTexture().copyToImage().saveToFile("plot.png"); // TODO: cin >> nome, tipo di file
                        std::cout << "plot saved" << std::endl; // TODO: aggiungere un testo su schermo
                    }
                }
            }

            window.clear(sf::Color::White);
            window.draw(axes);
            window.draw(vertexes);
            for (auto & l : labels)
            {
                window.draw(l);
            }
            window.display();
        }
    }

    auto plotter::get_plot_points() const
            -> std::vector<std::pair<int, int>>
    {
        namespace rvw = ranges::views;
        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_max] = std::minmax_element(_ys.begin(), _ys.end());
        auto const xs = _xs | rvw::transform(std::bind_front(&plotter::rescale, this, *x_min, *x_max, _width));
        auto const ys = _ys | rvw::transform(std::bind_front(&plotter::rescale, this, *y_min, *y_max, _height));

        return rvw::zip(xs, ys) | ranges::to<std::vector<std::pair<int, int>>>;
    }
} // namespace spl
