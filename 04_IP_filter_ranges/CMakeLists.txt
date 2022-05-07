cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

set(THIS_PROJECT ip_filter)
project(${THIS_PROJECT} VERSION ${PROJECT_VESRION})

add_executable(${THIS_PROJECT} main.cpp)
add_library(ip_filter_lib ip_filter.cpp)

set_target_properties(${THIS_PROJECT} ip_filter_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${THIS_PROJECT} PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(${THIS_PROJECT} ip_filter_lib)

find_package(range-v3 CONFIG REQUIRED)
target_link_libraries(${THIS_PROJECT} range-v3)
get_target_property(LIB_INCLUDE_DIR range-v3 INTERFACE_INCLUDE_DIRECTORIES)
include_directories( "${LIB_INCLUDE_DIR}" )

if (MSVC)
    target_compile_options(${THIS_PROJECT} PRIVATE
        /W4
    )
    target_compile_options(ip_filter_lib PRIVATE
        /W4
    )
else ()
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(ip_filter_lib PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()