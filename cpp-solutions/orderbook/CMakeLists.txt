cmake_minimum_required(VERSION 3.10)
project(orderbook)

set(CMAKE_CXX_STANDARD 17)

add_executable(orderbook main.cpp)

target_link_libraries(orderbook PRIVATE lz4)
