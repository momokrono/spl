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

// TODO: add stuff here
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

