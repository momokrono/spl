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

        sf::RenderWindow window(sf::VideoMode(_width, _height), "titolo");
        sf::VertexArray axes;

        sf::VertexArray vertexes{sf::LinesStrip};   // TODO: aggiungere la possibilità di cambiare primitiva
        axes.setPrimitiveType(sf::Lines);           // teper poter fare linee tratteggiate, cerchi, tratto punto, ecc
        
        auto points = get_plot_points();
        for (auto [x, y] : points)
        {
            sf::Vertex point;
            point.position = sf::Vector2f(x, _height-y);
            point.color = sf::Color::Blue;
            vertexes.append(point);
        }

        // TODO: abbellire sta roba, aggiungere le misure sugli assi

        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_may] = std::minmax_element(_ys.begin(), _ys.end());
        auto const x = plotter::rescale(*x_min, *x_max, _width, 0.);
        auto const y = plotter::rescale(*y_min, *y_may, _height, 0.);
        sf::Vertex vert;
        vert.color = sf::Color::Black;
        vert.position = sf::Vector2f(x,_border);
        axes.append(vert);
        vert.position = sf::Vector2f(x,_height-_border);
        axes.append(vert);
        vert.position = sf::Vector2f(_border,y);
        axes.append(vert);
        vert.position = sf::Vector2f(_width-_border,y);
        axes.append(vert);

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
                        texture.draw(axes);
                        texture.draw(vertexes);
                        texture.getTexture().copyToImage().saveToFile("plot.png"); // TODO: cin >> nome, tipo di file
                        std::cout << "plot saved" << std::endl; // TODO: aggiungere un testo su schermo
                    }
                }
            }

            window.clear(sf::Color::White);
            window.draw(axes);
            window.draw(vertexes);
            window.display();
        }
    }

    auto plotter::get_plot_points() const
            -> std::vector<std::pair<int, int>>
    {
        namespace rvw = ranges::views;
        auto const [x_min, x_max] = std::minmax_element(_xs.begin(), _xs.end());
        auto const [y_min, y_may] = std::minmax_element(_ys.begin(), _ys.end());
        auto const xs = _xs | rvw::transform(std::bind_front(&plotter::rescale, this, *x_min, *x_max, _width));
        auto const ys = _ys | rvw::transform(std::bind_front(&plotter::rescale, this, *y_min, *y_may, _height));

        return rvw::zip(xs, ys) | ranges::to<std::vector<std::pair<int, int>>>;

    }
} // namespace spl
