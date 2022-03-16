######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Wednesday Mar 16, 2022 10:12:02 CET
######################################################################

# examples/bench
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
if (build_examples_collection)
    add_executable(collection)

    target_compile_features(collection PUBLIC cxx_std_20)
    target_compile_options(collection PRIVATE -fconcepts-diagnostics-depth=2)
    target_link_options(collection PRIVATE)

    target_sources(collection PRIVATE example/collection.cpp)
    target_link_libraries(collection PRIVATE spl Threads::Threads CONAN_PKG::fmt project_warnings)
    target_include_directories(collection PUBLIC ./include/ ./include/3rd_party/)
endif()

# examples/viewport
if (build_examples_viewport)
    add_executable(viewport)

    target_compile_features(viewport PUBLIC cxx_std_20)
    target_compile_options(viewport PRIVATE -fconcepts-diagnostics-depth=2)
    target_link_options(viewport PRIVATE)

    target_sources(viewport PRIVATE example/viewport.cpp)
    target_link_libraries(viewport PRIVATE spl Threads::Threads CONAN_PKG::fmt project_warnings)
    target_include_directories(viewport PUBLIC ./include/)
endif()

# examples/effects
if (build_examples_effects)
    add_executable(effects)

    target_compile_features(effects PUBLIC cxx_std_20)
    target_compile_options(effects PRIVATE -fconcepts-diagnostics-depth=2)
    target_link_options(effects PRIVATE)

    target_sources(effects PRIVATE example/effects.cpp)
    target_link_libraries(effects PRIVATE spl Threads::Threads CONAN_PKG::fmt project_warnings)
    target_include_directories(effects PUBLIC ./include/)

    enable_sanitizers(effects)
endif()
