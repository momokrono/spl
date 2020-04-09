[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)

# `spl` - A Simple Plotting Library for C++

**The library is under heavy development, some parts are broken, others may not work from time to time.
If you find yourself having issues with it, feel free to report it.**


## Documentation

The functions `spl` now offer are limited to opening a window and plotting your data, and saving them to a file.

First of all, create an `spl::plotter` object, then to open a window simply call the `show` method.
If you like the way your data are plotted, just press *s* on your keyboard to save them to file.
Alternatively you can just `save_to_pmm` without needing a window at all.

Here is an example of how you can use it:

```cpp
int main()
{
    namespace rvw = ranges::views;
    auto xs = rvw::linear_distribute(-2*3.1415, 2*3.1415, 1000) | ranges::to_vector;
    auto ys = xs | rvw::transform([](auto x) { return std::cos(x); }) | ranges::to_vector;

    auto plot = spl::plotter{640, 480, xs, ys /* altri dati */};

    plot.save_as_pmm("my_plot.pmm"); // to save it directly

    plot.show();                     // to open a window
}
```