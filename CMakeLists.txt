cmake_minimum_required(VERSION 3.0)
project(reversi)

find_package(Boost 1.60.0 REQUIRED)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
