cmake_minimum_required(VERSION 3.11)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

set(THIS_PROJECT 12_Join_server)
project(${THIS_PROJECT} VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)

include(FetchContent)
FetchContent_Declare(sqlite3 URL "https://www.sqlite.org/snapshot/sqlite-snapshot-202205041843.tar.gz")
FetchContent_MakeAvailable(sqlite3)
if (NOT sqlite3_POPULATED)
  FetchContent_Populate(sqlite3)
endif()

option(WITH_GTEST "Whether to build google test" ON)

add_library(server_lib SHARED Server.cpp Server.h DB.h "${sqlite3_SOURCE_DIR}/sqlite3.c")
target_include_directories(server_lib PUBLIC ${Boost_INCLUDE_DIR} ${sqlite3_SOURCE_DIR})

add_executable(join_server ServerMain.cpp)
# add_executable(client ClientMain.cpp Client.h)

target_include_directories(join_server PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(join_server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} server_lib dl)
# target_link_libraries(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(join_server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# set_target_properties(client PROPERTIES
#     CXX_STANDARD 17
#     CXX_STANDARD_REQUIRED ON
# )

# target_include_directories(client PRIVATE "${CMAKE_BINARY_DIR}")

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
else ()
    set(MY_COMLILE_OPTIONS -Wall -Wextra -pedantic -Werror -Wshadow -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
endif()

target_compile_options(join_server PRIVATE ${MY_COMLILE_OPTIONS})
# target_compile_options(client PRIVATE ${MY_COMLILE_OPTIONS})

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

    target_link_libraries(tests gtest_main server_lib)
    target_compile_options(tests PRIVATE ${MY_COMLILE_OPTIONS})

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()