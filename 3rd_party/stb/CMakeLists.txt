######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Sunday Jun 14, 2020 22:42:16 CEST
######################################################################

add_library(stb OBJECT src/stb_impl.cpp)

target_include_directories(stb
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)
target_compile_options(stb PRIVATE -Wall -Wextra -Wpedantic)
