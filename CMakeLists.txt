cmake_minimum_required(VERSION 2.8.11)

project(cpp-tasks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

enable_testing()

find_package(Boost REQUIRED)
include_directories(include ${Boost_INCLUDE_DIR})

add_subdirectory(include)
add_subdirectory(test)
add_subdirectory(chat)