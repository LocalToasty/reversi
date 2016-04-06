cmake_minimum_required(VERSION 3.0)

add_executable(reversi
               main.cpp
               board.cpp
               reversi.cpp
               minimax.cpp)

set_property(TARGET reversi PROPERTY CXX_STANDARD 14)

install(TARGETS reversi RUNTIME DESTINATION bin)
