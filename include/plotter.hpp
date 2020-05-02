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
#include <variant>
#include <vector>

// if __cplusplus < 1322645?
#include <range/v3/range/concepts.hpp>

#include <range/v3/view/zip.hpp>
#include <range/v3/view/transform.hpp>
// endif

#include <SFML/Graphics.hpp>

namespace spl
{

struct format_result;

class plotter
{
    using primitive = std::variant<sf::VertexArray, sf::CircleShape>;
    using value_type = double;

    struct plot_spec
    {
        plot_spec(std::vector<value_type> x, std::vector<value_type> y, std::string format)
            : xs{std::move(x)}, ys{std::move(y)}, format{std::move(format)} {}
        std::vector<value_type> xs;
        std::vector<value_type> ys;
        std::string format;
    };

public:
    /// Constructors
    plotter() = default;
    plotter(
        uint_fast32_t w, uint_fast32_t h,
        std::vector<value_type> xs, std::vector<value_type> ys, std::string format = std::string{}
    ) : _width{w}, _height{h} { plot(std::move(xs), std::move(ys), format); }
    plotter(uint_fast32_t w, uint_fast32_t h) : _width{w}, _height{h} {}

    /// Add prepare a new set of points for plotting
    template <typename Container1, typename Container2>
        requires ranges::forward_range<Container1> and ranges::forward_range<Container2>
    auto plot(Container1 && xs, Container2 && ys, std::string format = std::string{}) -> plotter &;

    /// Open a window and show the plot
    void show() const;

    /// Save the plot in pmm file format
    bool save_as_pmm(std::filesystem::path const &) const; // default to PMM
    // maybe: std::string as_pmm_string() const;

    /// Save the plot as image
    bool save_plot(std::string const &) const;

private:
    uint_fast32_t _width  = 640;
    uint_fast32_t _height = 480;
    std::vector<plot_spec> _plots;
    uint_fast32_t _border = 10; // same for x and y
    mutable std::optional<sf::Texture> _cached_texture = std::nullopt;

    static format_result parse_format(std::string_view const format);
    /// Remap a point from [min, max] to [-length + border, length - border]
    constexpr
    int rescale(value_type const min, value_type const max, uint_fast32_t const length, value_type const x) const noexcept
    { return _border + (length - 2 * _border) / (max - min) * (x - min); }

    /// Remap two vectors (xs, ys) in a given range
    template <typename Pair, typename Container1, typename Container2>
        requires ranges::forward_range<Container1> and ranges::forward_range<Container2>
    auto get_plot_points(Pair const x_min_max, Pair const y_min_max, Container1 const & xs, Container2 const & ys) const
        -> std::vector<std::pair<int, int>>;

    /// Generate labels and axes primitives
    template <typename Pair>
    auto generate_labels(
        Pair const x_min_max, Pair const y_min_max,
        int const number_of_labels_x, int const number_of_labels_y, sf::Font const & font
    ) const
        -> std::pair<sf::VertexArray, std::vector<sf::Text>>;

    /// Generates primitives for points and lines
    template <typename Pair>
    auto build_primitives(Pair const x_min_max, Pair const y_min_max) const
        -> std::vector<primitive>;

    /// Generate a texture with the current content of `_plots`
    auto generate_texture() const -> sf::Texture;
    /// Returns the `_cached_texture`, eventually generating it not present
    auto get_texture() const -> sf::Texture const &;
};

struct format_error : std::exception
{
    auto what() -> char const * { return "bad format string"; }
};

enum class line  : uint8_t { solid, dashed, dash_dot, dotted, no_line };
struct format_result
{
    sf::Color color = sf::Color::Blue;
    int8_t figure_code = 0;
    line line_type = line::solid;
};

} // namespace spl

namespace spl
{
template <typename Pair, typename Container1, typename Container2>
    requires ranges::forward_range<Container1> and ranges::forward_range<Container2>
auto plotter::get_plot_points(Pair const x_min_max, Pair const y_min_max, Container1 const & xs, Container2 const & ys
) const
        -> std::vector<std::pair<int, int>>
{
    namespace rvw = ranges::views;
    auto const [x_min, x_max] = x_min_max;
    auto const [y_min, y_max] = y_min_max;
    auto const xs_f = xs | rvw::transform(std::bind_front(&plotter::rescale, this, x_min, x_max, _width));
    auto const ys_f = ys | rvw::transform(std::bind_front(&plotter::rescale, this, y_min, y_max, _height));

    return rvw::zip(xs_f, ys_f) | ranges::to<std::vector<std::pair<int, int>>>;
}

template <typename Container1, typename Container2>
            requires ranges::forward_range<Container1> && ranges::forward_range<Container2>
auto plotter::plot(Container1 && xs, Container2 && ys, std::string format)
    -> plotter &
{
    _cached_texture = std::nullopt;
    if constexpr (std::is_same_v<std::decay_t<Container1>, std::vector<double>>) {
        if constexpr (std::is_same_v<std::decay_t<Container2>, std::vector<double>>) {
            _plots.emplace_back(std::forward<Container1>(xs), std::forward<Container2>(ys), std::move(format));
        } else {
            _plots.emplace_back(
                std::forward<Container1>(xs), std::vector<double>(ys.begin(), ys.end()), std::move(format)
            );
        }
    } else if constexpr (std::is_same_v<std::decay_t<Container2>, std::vector<double>>) {
        _plots.emplace_back(
            std::vector<double>(xs.begin(), xs.end()),
            std::forward<Container2>(ys),
            std::move(format)
        );

    } else {
        _plots.emplace_back(
            std::vector<double>(xs.begin(), xs.end()),
            std::vector<double>(ys.begin(), ys.end()),
            std::move(format)
        );
    }

    return *this;
}


#warning complete this function
template <typename Pair>
auto plotter::build_primitives(Pair const x_min_max, Pair const y_min_max) const
    -> std::vector<primitive>
{
    auto plot_points = std::vector<primitive>{};
    for (auto const & [xs, ys, format] : _plots) {
        auto const [color, figure_code, line] = parse_format(format);

        auto const points = get_plot_points(x_min_max, y_min_max, xs, ys);
        if (line == line::solid) {
            auto vertex_array = sf::VertexArray{sf::LineStrip};
            for (auto const [x, y] : points) {
                vertex_array.append({{x + 0.f, _height - y + 0.f}, color});
            }
            plot_points.push_back(std::move(vertex_array));
        }
        if (line == line::no_line and figure_code == 0) {
            auto vertex_array = sf::VertexArray{sf::Points};
            for (auto const [x, y] : points) {
                vertex_array.append({{x + 0.f, _height - y + 0.f}, color});
            }
            plot_points.push_back(std::move(vertex_array));
        } else if (figure_code > 0) {
            for (auto const [x, y] : points) {
                constexpr auto radius = 5.f;
                auto vertex_icon = sf::CircleShape{radius};
                vertex_icon.setOrigin(radius, radius);
                if (figure_code > 2) {
                    vertex_icon.setPointCount(figure_code);
                    if (figure_code == 4) {
                        vertex_icon.rotate(45);
                    }
                } else if (figure_code == 2) {
                    vertex_icon.setRadius(2*radius);
                    vertex_icon.setOrigin(2*radius, 2*radius);
                }
                vertex_icon.setFillColor(color);
                vertex_icon.setPosition(x, _height - y);
                plot_points.push_back(vertex_icon);
            }
        }
    }

    return plot_points;
}
} // namespace spl

#endif /* PLOTTER_HPP */

