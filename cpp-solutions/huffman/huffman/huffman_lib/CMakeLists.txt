cmake_minimum_required(VERSION 3.21)
project(huffman-lib)
set(CMAKE_CXX_STANDARD 17)

add_library(huffman STATIC
        bit_reader.cpp
        bit_writer.cpp
        huffman_lib.cpp
        huffman_tree.cpp
        serialization.cpp
        bit_code.cpp
        counter.cpp)

target_include_directories(huffman PUBLIC include)
