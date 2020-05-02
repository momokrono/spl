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
    auto xs = rvw::linear_distribute(-2*3.1415, 2*3.1415, 1000) | ranges::to_vector;
    auto ys = xs | rvw::transform([](auto x) { return std::cos(x); }) | ranges::to_vector;

    auto plot = spl::plotter{640, 480, xs, ys /* format here, eventually */};

    plot.save_plot("my_plot.pmm"); // to save it directly

    plot.show();                     // to open a window
}
```
You can nest more plots each one with different format, for example
```cpp
plot.plot(x1,y1)
    .plot(x2,y2,"gt-")
    .plot(x3,y3,"bo ")
    ;
```
and then show it, or save it.