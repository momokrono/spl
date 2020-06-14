# Configuration for cmake

# Default build type
if (NOT CMAKE_BUILD_TYPE)
    message(
        STATUS "Setting build type to default 'RelWithDebugInfo' ")
    set(CMAKE_BUILD_TYPE RelWithDebugInfo CACHE STRING "Choose the type of build: " FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebugInfo")
endif()

# ccache
option(ENABLE_CACHE "Enable ccache if available" ON)
if (NOT ENABLE_CACHE)
    return()
endif()

find_program(CACHE_BINARY "ccache")
if (CACHE_BINARY)
    message(STATUS "ccache found and enabled")
    set (CMAKE_CXX_COMPILER_LAUNCHER "ccache")
else()
    message(WARNING "ccache is enabled but was not found. It will not be used")
endif()

# compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    message(STATUS "Exporting compile commands")
else()
    message(STATUS "Not exporting compile commands")
endif()

# sanitizers
function(enable_sanitizers project_name)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(SANITIZERS "")

        option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" FALSE)
        if (ENABLE_SANITIZER_ADDRESS)
            list(APPEND SANITIZERS "address")
        endif()

        option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
        if (ENABLE_SANITIZER_MEMORY)
            list(APPEND SANITIZERS "memory")
        endif()

        option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
            "Enable undefined behavior sanitizer" FALSE)
        if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
            list(APPEND SANITIZERS "undefined")
        endif()

        option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
        if(ENABLE_SANITIZER_THREAD)
            list(APPEND SANITIZERS "thread")
        endif()

        list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)
    endif()

    if(LIST_OF_SANITIZERS)
        if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
            target_compile_options(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
            target_link_libraries(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
        endif()
    endif()
endfunction()

# lto
function(enable_lto project_name)
    option(ENABLE_LTO "Enable link time optimization" OFF)
    if (ENABLE_LTO)
        check_ipo_supported(RESULT result OUTPUT output)
        if(result)
            set_property(TARGET ${project_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        else()
            message(WARNING "IPO is not supported: ${output}")
        endif()
    endif()
endfunction()

