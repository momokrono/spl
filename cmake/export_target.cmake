# Packaging: stackoverflow, making-a-cmake-library-accessible-by-other-cmake-packages-automatically
# CMakeParseArguments: https://asitdhal.medium.com/cmake-functions-and-macros-22293041519f

include_guard()

function(export_target)
    set(prefix _EXPORT)
    set(flags)
    set(singleValues PACKAGE_NAME NAMESPACE)
    set(multiValues TARGETS INCLUDE_DIRECTORIES)

    if (_EXPORT_PACKAGE_NAME STREQUAL "")
        message(SEND_ERROR "the function `export_target` needs a PACKAGE_NAME parameter")
    endif()
    if (_EXPORT_TARGETS STREQUAL "")
        message(SEND_ERROR "the function `export_target` needs at least a TARGETS parameter")
    endif()
    if (_EXPORT_INCLUDE_DIRECTORIES STREQUAL "")
        message(
            SEND_ERROR "the function `export_target`needs at least a INCLUDE_DIRECTORIES parameter"
        )
    endif()


    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})

    file(
        WRITE "${CMAKE_CURRENT_BINARY_DIR}/${_EXPORT_PACKAGE_NAME}Config.cmake"
        "
            include(\"\$\{CMAKE_CURRENT_LIST_DIR\}/${_EXPORT_PACKAGE_NAME}Targets.cmake\")
            set_property(
                TARGET ${_EXPORT_PACKAGE_NAME}
                APPEND PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES ${_EXPORT_INCLUDE_DIRECTORIES}
            )
        "
    )

    if ("${_EXPORT_NAMESPACE}" STREQUAL "")
        export(
            TARGETS ${_EXPORT_TARGETS}
            FILE "${CMAKE_CURRENT_BINARY_DIR}/${_EXPORT_PACKAGE_NAME}Targets.cmake"
        )
    else()
        export(
            TARGETS ${_EXPORT_TARGETS}
            NAMESPACE ${_EXPORT_NAMESPACE}
            FILE "${CMAKE_CURRENT_BINARY_DIR}/${_EXPORT_PACKAGE_NAME}Targets.cmake"
        )
    endif()
endfunction()
