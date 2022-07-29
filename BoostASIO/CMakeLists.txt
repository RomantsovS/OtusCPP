cmake_minimum_required(VERSION 3.5)

project(26.asio)

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)
include_directories(${Boost_INCLUDE_DIR})

add_executable(asio_timer asio_timer.cpp)

set_target_properties(asio_timer PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_timer
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )


add_executable(asio_resolver asio_resolver.cpp)

set_target_properties(asio_resolver PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_resolver
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )


add_executable(asio_client asio_client.cpp)

set_target_properties(asio_client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_client
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )


add_executable(asio_server asio_server.cpp)

set_target_properties(asio_server PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )


add_executable(asio_async_client asio_async_client.cpp)

set_target_properties(asio_async_client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_async_client
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

add_executable(asio_async_server asio_async_server.cpp)

set_target_properties(asio_async_server PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )

target_link_libraries(asio_async_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

add_executable(chat_server chat_server.cpp)

set_target_properties(chat_server PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )
target_link_libraries(chat_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

add_executable(chat_client chat_client.cpp)

set_target_properties(chat_client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        )
target_link_libraries(chat_client
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )