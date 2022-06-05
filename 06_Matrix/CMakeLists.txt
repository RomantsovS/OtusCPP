cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

set(THIS_PROJECT matrix)
project(${THIS_PROJECT} VERSION ${PROJECT_VESRION})

option(WITH_GTEST "Whether to build google test" ON)

add_executable(${THIS_PROJECT} main.cpp)

set_target_properties(${THIS_PROJECT} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(${THIS_PROJECT} PRIVATE "${CMAKE_BINARY_DIR}")

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/0affd45b62371c1de44064a5a5f8817242208442.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(tests tests.cpp)

    set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(tests gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

if (MSVC)
    target_compile_options(${THIS_PROJECT} PRIVATE
        /W4
    )
else ()
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()