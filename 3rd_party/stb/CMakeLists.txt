######################################################################
# @author      : rbrugo, momokrono
# @file        : CMakeLists
# @created     : Sunday Jun 14, 2020 22:42:16 CEST
######################################################################

add_library(stb OBJECT src/stb_impl.cpp)

target_include_directories(stb PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_compile_options(stb PRIVATE -Wall -Wextra -Wpedantic -fconcepts)
# target_link_libraries(stb PRIVATE)
# target_link_options(stb PRIVATE)
# set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=unreachable -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=alignment")
# set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fsanitize=null -fsanitize=unreachable -fsanitize=return -fsanitize=signed-integer-overflow -fsanitize=bounds -fsanitize=alignment")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")


# target_sources(stb PRIVATE src/stb_impl.cpp)

