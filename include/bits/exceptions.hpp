/**
 * @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
 * @file        : exceptions
 * @created     : Wednesday Jun 03, 2020 22:29:53 CEST
 * @license     : MIT
 * */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <string>
#include <fmt/format.h>

namespace spl
{

struct out_of_range : std::exception
{
    std::string message;
    out_of_range(size_t x, size_t w)
    {
        message = fmt::format("out of range: required element {} but size is {}", x, w);
    }
    out_of_range(size_t x, size_t y, size_t w, size_t h)
    {
        message = fmt::format("out of range: required element ({}, {}) but size is ({}, {})", x, y, w, h);
    }

    auto what() const noexcept -> char const * override 
    { return message.c_str(); }
};


} // namespace spl

#endif /* EXCEPTIONS_HPP */
