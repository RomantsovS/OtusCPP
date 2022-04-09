cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VESRION})

option(WITH_GTEST "Whether to build google test" ON)

add_executable(ip_filter main.cpp)
add_library(ip_filter_lib ip_filter.cpp)

set_target_properties(ip_filter ip_filter_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(ip_filter PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(ip_filter PRIVATE ip_filter_lib
)

if(WITH_GTEST)
    include(FetchContent)
    FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/0affd45b62371c1de44064a5a5f8817242208442.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(tests tests.cpp)
    target_link_libraries(tests gtest_main ip_filter_lib)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(test_version PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(ip_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(ip_filter_lib PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)