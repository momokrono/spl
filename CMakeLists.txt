######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Tuesday Mar 31, 2020 00:22:53 CEST
######################################################################

cmake_minimum_required(VERSION 3.16.2)

project(spl CXX)
include(cmake/general.cmake)

# Various libraries
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS) # enables the import of single libraries
find_package(Threads REQUIRED)
add_subdirectory(3rd_party/stb)

# Enables warning
add_library(project_warnings INTERFACE)
include(cmake/compiler_warnings.cmake)
set_project_warnings(project_warnings)

# spl library
add_library(spl)
target_sources(spl PRIVATE
    src/graphics/image.cpp
    src/graphics/primitive.cpp
)
target_compile_features(spl PUBLIC cxx_std_20)
target_link_options(spl PRIVATE)
target_link_libraries(spl PRIVATE project_warnings CONAN_PKG::fmt PUBLIC stb ) # range-v3 is currently not needed
target_include_directories(spl
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/spl"
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
enable_sanitizers(spl)

# examples/bench
option(build_examples_bench "Build the bench test (not a real benchmark, used during development)" TRUE)
if (build_examples_bench)
    add_executable(bench)

    target_compile_features(bench PUBLIC cxx_std_20)
    target_compile_options(bench PRIVATE -Wall -Wextra -Wpedantic -fconcepts)
    target_link_options(bench PRIVATE)

    target_sources(bench PRIVATE example/bench.cpp)
    target_link_libraries(bench PRIVATE spl ${CONAN_LIBS})
    target_include_directories(bench PUBLIC ./include/spl ./include/ ./include/3rd_party/)
endif()

# examples/graphics
option(build_examples_graphics "Build the graphics example" TRUE)
if (build_examples_graphics)
    add_executable(graphics)

    target_compile_features(graphics PUBLIC cxx_std_20)
    target_compile_options(graphics PRIVATE -Wall -Wextra -Wpedantic -fconcepts)
    target_link_options(graphics PRIVATE)

    target_sources(graphics PRIVATE example/graphics.cpp)
    target_link_libraries(graphics PRIVATE spl ${CONAN_LIBS} ${Threads} -lsfml-system -lsfml-graphics -lsfml-window)
    target_include_directories(graphics PUBLIC ./include/spl ./include/ ./include/3rd_party/)
endif()




# target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spl ${CONAN_LIBS} ${Threads} -lsfml-system -lsfml-graphics -lsfml-window)
