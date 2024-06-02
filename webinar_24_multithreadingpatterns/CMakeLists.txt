cmake_minimum_required(VERSION 3.5)
project(21_MultyPattern)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(barrier barrier.cpp)
set_target_properties(barrier PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(barrier
                ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(nuclear_reaction nuclear_reaction.cpp)
set_target_properties(nuclear_reaction PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(nuclear_reaction
		${CMAKE_THREAD_LIBS_INIT}
)


add_executable(double_checked_locking double_checked_locking.cpp)
set_target_properties(double_checked_locking PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(double_checked_locking
		${CMAKE_THREAD_LIBS_INIT}
)


add_executable(read_write_lock read_write_lock.cpp)
set_target_properties(read_write_lock PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(read_write_lock
		${CMAKE_THREAD_LIBS_INIT}
)


add_executable(guarded_suspension guarded_suspension.cpp)
set_target_properties(guarded_suspension PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(guarded_suspension
		${CMAKE_THREAD_LIBS_INIT}
)


add_executable(active_object active_object.cpp)
set_target_properties(active_object PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(active_object
		${CMAKE_THREAD_LIBS_INIT}
)

add_executable(thread_pool thread_pool.cpp)
set_target_properties(thread_pool PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)
target_link_libraries(thread_pool
                ${CMAKE_THREAD_LIBS_INIT}
)
