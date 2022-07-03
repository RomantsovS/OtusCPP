cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

find_package(Boost COMPONENTS program_options REQUIRED)

set(THIS_PROJECT bayan)
project(${THIS_PROJECT} VERSION ${PROJECT_VESRION})

file(GLOB sources
  *.cpp
  *.h
)

add_executable(${THIS_PROJECT} ${sources})

set_target_properties(${THIS_PROJECT} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${THIS_PROJECT} PRIVATE "${CMAKE_BINARY_DIR}")

target_link_libraries(${THIS_PROJECT} PRIVATE ${Boost_LIBRARIES})
target_include_directories(${THIS_PROJECT} PRIVATE ${Boost_INCLUDE_DIRS})

if (MSVC)
    target_compile_options(${THIS_PROJECT} PRIVATE
        /W4
    )
else ()
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()