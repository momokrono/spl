######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Tuesday Mar 31, 2020 00:22:53 CEST
######################################################################
cmake_minimum_required(VERSION 3.16.2)

project(spt CXX)
add_executable(${CMAKE_PROJECT_NAME})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
find_package(Threads REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -fconcepts)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=unreachable -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=alignment")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=unreachable -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=alignment")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")


target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/main.cpp src/plotter.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CONAN_LIBS} ${Threads} -lsfml-system -lsfml-graphics -lsfml-window)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ./include)

