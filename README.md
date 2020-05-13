[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)

# `spl` - A Simple Plotting Library for C++

**The library is under heavy development, some parts are broken, others may not work from time to time.
If you find yourself having issues with it, feel free to report it.**


## Documentation

The functions `spl` now offer are limited to opening a window and plotting your data, and saving them to a file.

First of all, create an `spl::plotter` object, then to open a window simply call the `spl::show` method.
To plot your data you can either use the constructor or use `spl::plot`.
If you like the way your data are plotted, just press *s* on your keyboard while the window is open, or use `spl::save_plot` to save them to file.

Here is an example of how you can use it:

```cpp
int main()
{
    namespace rvw = ranges::views;

    // Generate some points to plot
    auto x1 = rvw::linear_distribute(-2*pi, 2*pi, 400) | ranges::to_vector;
    auto y1 = x1 | rvw::transform([](auto x) { return std::exp(x) / 1e2; }) | ranges::to_vector;
    auto x2 = rvw::linear_distribute(-4*pi, 4*pi, 100) | ranges::to_vector;
    auto y2 = x2 | rvw::transform([](auto x) { return std::sin(x); }) | ranges::to_vector;

    auto graph = spl::plotter{640, 480 /*, xs, ys, format*/};

    graph.plot(x1, y1);           // plot x1 and y1 with default format
    graph.plot(x2, y2, "ro-");    // plot x2 and y2 with a custom format

    graph.show();                    // to open a window
    // graph.save_plot("plot.png");  // to save directly
}
```
The code above gives the following output:
![example](https://github.com/momokrono/spl/blob/master/plot.png)

You can chain more plots each one with different format, for example
```cpp
graph.plot(x1,y1)
     .plot(x2,y2,"gt-")
     .plot(x3,y3,"bo ")
     ;
```
and then show it, or save it.

### Plot format

A format string let you define three properties of the plot (the color, the primitive and the line
type) in a way similar to matplotlib.
Currently available options are:
- Colors
    + `b`: blue (default)
    + `g`: green
    + `r`: red
    + `y`: yellow
    + `m`: magenta
    + `k`|`B`: black
- Primitives:
    + `.`: pixel (default)
    + `o`: small circle
    + `O`: big circle
    + `t`: triangle
    + `s`: square
    + `p`: pentagon
    + `e`: exagon
- Line types:
    + `-`: solid (default)
    + ` `: no line
