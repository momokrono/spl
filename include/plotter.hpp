/**
 * @author      : rbrugo, momokrono
 * @file        : plotter
 * @created     : Tuesday Mar 31, 2020 00:06:27 CEST
 * @license     : MIT
 * */

#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <filesystem>
#include <cstdint>
#include <vector>

#include <SFML/Graphics.hpp>

namespace spl
{
    class plotter
    {
    public:
        plotter(uint_fast32_t w, uint_fast32_t h, std::vector<double> xs, std::vector<double> ys)
            : _width{w}, _height{h}, _xs(std::move(xs)), _ys(std::move(ys))
        {}
        plotter(uint_fast32_t w, uint_fast32_t h) : plotter{w, h, {}, {}} {}

        void show() const;
        bool save_as_pmm(std::filesystem::path const &) const; // default to PMM
        // maybe: std::string as_pmm_string() const;
        bool save_canvas(std::pair<sf::VertexArray, sf::VertexArray> const &, std::vector<sf::Text> const &, std::string const &) const;

    private:
        uint_fast32_t _width;
        uint_fast32_t _height;

        std::vector<double> _xs;
        std::vector<double> _ys;

        uint_fast32_t _border = 10; // same for x and y

        constexpr
        auto rescale(double const min, double const max, uint_fast32_t const length, double const x) const noexcept
            -> int
        {
            return _border + (length - 2 * _border) / (max - min) * (x - min);
        }

        auto get_plot_points() const -> std::vector<std::pair<int, int>>;
        auto generate_labels(int, int, sf::Font const &) const -> std::pair<sf::VertexArray, std::vector<sf::Text>>;

    };
} // namespace spl

#endif /* PLOTTER_HPP */

