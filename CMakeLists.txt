######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Tuesday Mar 31, 2020 00:22:53 CEST
######################################################################

cmake_minimum_required(VERSION 3.16.2)

project(spl VERSION 0.4.1 LANGUAGES CXX)
include(cmake/general.cmake)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                                 Conan                                  #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
include(cmake/conan.cmake)
conan_cmake_configure(
    BASIC_SETUP
    CMAKE_TARGETS
    REQUIRES fmt/8.1.1
    GENERATORS cmake_find_package)
conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_BINARY_DIR}
    BUILD missing
    GENERATOR cmake
    SETTINGS ${settings}
)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
find_package(fmt REQUIRED)

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
target_sources(spl
    PRIVATE
        src/image.cpp
        src/viewport.cpp
        src/primitive.cpp
        src/text.cpp
        src/effects.cpp
)
target_compile_features(spl PUBLIC cxx_std_20)
target_link_options(spl PRIVATE RELEASE)
target_link_libraries(spl PRIVATE project_warnings fmt::fmt stb)
target_include_directories(spl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)
enable_sanitizers(spl)
enable_lto(spl)

option(SPL_FILL_MULTITHREAD "image::fill uses threads to fill faster" OFF)
option(SPL_DISABLE_GAMMA_CORRECTION
    "color-composing algorithms like `over` will not use gamma correction" OFF
)
option(build_examples_bench "Build the bench test (not a real benchmark, used as sandbox)" TRUE)
option(build_examples_graphics "Build the graphics example" TRUE)
option(build_examples_blur "Build the blur example" TRUE)
option(build_examples_collection "Build the collection example" TRUE)
option(build_examples_viewport "Build the viewport example" TRUE)
option(build_examples_effects "Build the effects example" TRUE)
option(build_examples_text "Build the text example" TRUE)

if (SPL_FILL_MULTITHREAD)
    message(STATUS "FILL_MULTITHREADED option enabled")
    target_compile_definitions(spl PUBLIC -DSPL_FILL_MULTITHREAD)
    target_link_libraries(spl PRIVATE Threads::Threads)
endif()
if (SPL_DISABLE_GAMMA_CORRECTION)
    message(STATUS "gamma correction option enabled")
    target_compile_definitions(spl PUBLIC -DSPL_DISABLE_GAMMA_CORRECTION)
endif()
add_library(spl::spl ALIAS spl)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                            build examples                            #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/example/")

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                              packaging                               #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    PATH_VARS CMAKE_INSTALL_LIBDIR)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY SameMajorVersion
)

install(TARGETS spl project_warnings stb ${build_targets_list} EXPORT ${PROJECT_NAME}-targets)

install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    NAMESPACE spl::
    FILE "${PROJECT_NAME}-targets.cmake"
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
