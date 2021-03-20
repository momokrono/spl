# Configuration for cmake

include_guard()

# Default build type
if (NOT CMAKE_BUILD_TYPE)
    message(
        STATUS "Setting build type to default 'RelWithDebInfo' ")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build: " FORCE)
    set_property(
        CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    )
endif()

set(CMAKE_CXX_FLAGS_RELEASE -O2)
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

# ccache
option(ENABLE_CACHE "Enable ccache if available" ON)
if (ENABLE_CACHE)
    find_program(CACHE_BINARY "ccache")
    if (CACHE_BINARY)
        message(STATUS "ccache found and enabled")
        set (CMAKE_CXX_COMPILER_LAUNCHER "ccache")
    else()
        message(WARNING "ccache is enabled but was not found. It will not be used")
    endif()
endif()

# compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
    message(STATUS "Exporting compile commands")
else()
    message(STATUS "Not exporting compile commands")
endif()

# sanitizers
function(enable_sanitizers target_name)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(SANITIZERS "")

        string(TOUPPER "ENABLE_SANITIZER_ADDRESS_FOR_${target_name}" ENABLE_SANITIZER_ADDRESS)
        option(${ENABLE_SANITIZER_ADDRESS} "Enable address sanitizer for ${target_name}" FALSE)
        if (${ENABLE_SANITIZER_ADDRESS})
            list(APPEND SANITIZERS "address")
        endif()

        string(TOUPPER "ENABLE_SANITIZER_MEMORY_FOR_${target_name}" ENABLE_SANITIZER_MEMORY)
        option(${ENABLE_SANITIZER_MEMORY} "Enable memory sanitizer for ${target_name}" FALSE)
        if (${ENABLE_SANITIZER_MEMORY})
            list(APPEND SANITIZERS "memory")
        endif()

        string(TOUPPER "ENABLE_SANITIZER_UNDEFINED_BEHAVIOR_FOR_${target_name}" ENABLE_SANITIZER_UB)
        option(
            ${ENABLE_SANITIZER_UB} "Enable undefined behavior sanitizer for ${target_name}" FALSE
        )
        if(${ENABLE_SANITIZER_UB})
            list(APPEND SANITIZERS "undefined")
        endif()

        string(TOUPPER "ENABLE_SANITIZER_THREAD_FOR_${target_name}" ENABLE_SANITIZER_THREAD)
        option(${ENABLE_SANITIZER_THREAD} "Enable thread sanitizer for ${target_name}" FALSE)
        if(${ENABLE_SANITIZER_THREAD})
            list(APPEND SANITIZERS "thread")
        endif()

        list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)
    endif()

    if(LIST_OF_SANITIZERS)
        if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
            target_compile_options(${target_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
            target_link_libraries(${target_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
        endif()
    endif()
endfunction()

# lto
include(CheckIPOSupported)
function(enable_lto target_name)
    string(TOUPPER "ENABLE_LTO_FOR_${target_name}" OPT_NAME)
    option(${OPT_NAME} "Enable link time optimization" OFF)
    if (${OPT_NAME})
        check_ipo_supported(RESULT result OUTPUT output)
        if(result)
            set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        else()
            message(WARNING "LTO is not supported: ${output}")
        endif()
    endif()
endfunction()

# force colors in compiler output
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
mark_as_advanced(FORCE_COLORED_OUTPUT)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()
