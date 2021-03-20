# Utility functions
include_guard()

function(add_example)
    set(prefix EX)
    set(flags)
    set(single NAME DESCRIPTION)
    set(multi  LINK_LIBRARIES INCLUDE_DIRECTORIES SOURCES)

    cmake_parse_arguments(${prefix} "${flags}" "${single}" "${multi}" ${ARGN})

    if (EX_NAME STREQUAL "")
        message(ERROR "You must pass a NAME argument to add_example function")
        return()
    endif()

    if (EX_DESCRIPTION STREQUAL "")
        set(EX_DESCRIPTION "Build the ${EX_NAME} example")
    endif()

    option("build_example_${EX_NAME}" "${EX_DESCRIPTION}" TRUE)
    if (${build_example_${EX_NAME}})
        add_executable(${EX_NAME})

        target_compile_features(${EX_NAME} PUBLIC cxx_std_20)
        target_compile_options(${EX_NAME} PRIVATE -fconcepts-diagnostics-depth=2)
        target_link_options(${EX_NAME} PRIVATE)

        target_sources(${EX_NAME} PRIVATE ${EX_SOURCES})
        target_link_libraries(${EX_NAME} PRIVATE ${EX_LINK_LIBRARIES})
        target_include_directories(${EX_NAME} PRIVATE ${EX_INCLUDE_DIRECTORIES})
    endif()
endfunction()
