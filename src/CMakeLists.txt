cmake_minimum_required(VERSION 3.0)

find_package(Boost 1.60 COMPONENTS program_options REQUIRED) 
include_directories(${Boost_INCLUDE_DIR})
add_executable(reversi
               main.cpp
               board.cpp
               human.cpp
               minimax.cpp
               reversi.cpp)
target_link_libraries(reversi ${Boost_LIBRARIES})
set_property(TARGET reversi PROPERTY CXX_STANDARD 14)

install(TARGETS reversi RUNTIME DESTINATION bin)
