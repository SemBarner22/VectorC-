cmake_minimum_required(VERSION 3.13)
project(exam_vector)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -fsanitize=address -fsanitize=leak -Wall -std=c++11 -pedantic -D_GLIBCXX_DEBUG")
endif()

set(CMAKE_CXX_STANDARD 17)
add_executable(
        sorokin-testing
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest-vector.cc
        vector.h
        fault_injection.cpp
        fault_injection.h
        counted.cpp
        counted.h
        vector_testing.cpp
        vector_iterator.h)

target_link_libraries(sorokin-testing -lpthread)