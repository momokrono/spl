######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Tuesday Mar 31, 2020 00:22:53 CEST
######################################################################

cmake_minimum_required(VERSION 3.16.2)

project(spl VERSION 0.2 LANGUAGES CXX)
include(cmake/general.cmake)
include(cmake/export_target.cmake)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                                 Conan                                  #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
include(cmake/conan.cmake)
conan_cmake_configure(REQUIRES fmt/7.1.2 BASIC_SETUP CMAKE_TARGETS)
conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_BINARY_DIR}
    BUILD missing
    GENERATOR cmake
    SETTINGS ${settings}
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS) # enables the import of single libraries

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                              Threads, stb                              #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
find_package(Threads REQUIRED)
add_subdirectory(3rd_party/stb)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                            Enable warnings                             #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
add_library(project_warnings INTERFACE)
include(cmake/compiler_warnings.cmake)
set_project_warnings(project_warnings)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                                  SPL                                   #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
add_library(spl)
target_sources(spl PRIVATE src/image.cpp src/primitive.cpp)
target_compile_features(spl PUBLIC cxx_std_20)
target_link_options(spl PRIVATE RELEASE)
target_link_libraries(spl PRIVATE project_warnings CONAN_PKG::fmt stb)
target_include_directories(spl
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
enable_sanitizers(spl)
enable_lto(spl)
option(SPL_FILL_MULTITHREAD "image::fill uses threads to fill faster" OFF)
if (SPL_FILL_MULTITHREAD)
    message(STATUS "FILL_MULTITHREADED option enabled")
    target_compile_definitions(spl PUBLIC -DSPL_FILL_MULTITHREAD)
    target_link_libraries(spl PRIVATE Threads::Threads)
endif()
add_library(spl::spl ALIAS spl)
export_target(
    PACKAGE_NAME spl
    TARGETS spl project_warnings stb
    NAMESPACE spl
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)

# TODO: move in a subdirectory

# examples/bench
option(
    build_examples_bench
    "Build the bench test (not a real benchmark, used during development)"
    TRUE
)
if (build_examples_bench)
    add_executable(bench)

    target_compile_features(bench PUBLIC cxx_std_20)
    target_compile_options(bench PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(bench PRIVATE -pthread)

    target_sources(bench PRIVATE example/bench.cpp)
    target_link_libraries(bench PRIVATE spl CONAN_PKG::fmt)
    target_include_directories(bench PUBLIC ./include/ ./include/3rd_party/)
endif()

# examples/graphics
option(build_examples_graphics "Build the graphics example" TRUE)
if (build_examples_graphics)
    add_executable(graphics)

    target_compile_features(graphics PUBLIC cxx_std_20)
    target_compile_options(graphics PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(graphics PRIVATE)

    target_sources(graphics PRIVATE example/graphics.cpp)
    target_link_libraries(graphics PRIVATE spl CONAN_PKG::fmt Threads::Threads)
    target_include_directories(graphics PUBLIC ./include/ ./include/3rd_party/)
endif()

# examples/blur
option(build_examples_blur "Build the blur example" TRUE)
if (build_examples_blur)
    add_executable(blur)

    target_compile_features(blur PUBLIC cxx_std_20)
    target_compile_options(blur PRIVATE)
    target_link_options(blur PRIVATE)

    target_sources(blur PRIVATE example/blur.cpp)
    target_link_libraries(blur PRIVATE spl Threads::Threads CONAN_PKG::fmt project_warnings)
    target_include_directories(blur PUBLIC ./include/ ./include/3rd_party/)
endif()

# examples/collection
option(build_examples_collection "Build the collection example" TRUE)
if (build_examples_collection)
    add_executable(collection)

    target_compile_features(collection PUBLIC cxx_std_20)
    target_compile_options(collection PRIVATE -fconcepts-diagnostics-depth=2)
    target_link_options(collection PRIVATE)

    target_sources(collection PRIVATE example/collection.cpp)
    target_link_libraries(collection PRIVATE spl Threads::Threads CONAN_PKG::fmt project_warnings)
    target_include_directories(collection PUBLIC ./include/ ./include/3rd_party/)
endif()
