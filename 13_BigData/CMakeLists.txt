cmake_minimum_required(VERSION 3.11)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

set(THIS_PROJECT 13_BigData)
project(${THIS_PROJECT} VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)

option(WITH_GTEST "Whether to build google test" ON)

add_executable(mapreduce ./homework/client.cpp ./homework/mapreduce.h)

add_executable(mapreduce_example MapReduce.cpp)

target_include_directories(mapreduce PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(mapreduce ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(mapreduce PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(mapreduce_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
else ()
    set(MY_COMLILE_OPTIONS -Wall -Wextra -pedantic -Werror -Wshadow -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
endif()

target_compile_options(mapreduce PRIVATE ${MY_COMLILE_OPTIONS})
target_compile_options(mapreduce_example PRIVATE ${MY_COMLILE_OPTIONS})

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/0affd45b62371c1de44064a5a5f8817242208442.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(tests ./homework/tests.cpp)

    set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(tests gtest_main)
    target_compile_options(tests PRIVATE ${MY_COMLILE_OPTIONS})

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()