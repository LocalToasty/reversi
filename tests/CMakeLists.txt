cmake_minimum_required(VERSION 3.0)

enable_testing()

include_directories(${reversi_SOURCE_DIR}/src)

add_executable(test_board EXCLUDE_FROM_ALL
               test_board.cpp
               ../src/board.cpp)
set_property(TARGET test_board PROPERTY CXX_STANDARD 14)
add_test(test_board test_board)

add_executable(test_minimax EXCLUDE_FROM_ALL
               test_minimax.cpp
               ../src/minimax.cpp
               ../src/board.cpp)
set_property(TARGET test_minimax PROPERTY CXX_STANDARD 14)
add_test(test_minimax test_minimax)

add_executable(test_reversi EXCLUDE_FROM_ALL
               test_reversi.cpp
               ../src/reversi.cpp
               ../src/board.cpp)
set_property(TARGET test_reversi PROPERTY CXX_STANDARD 14)
add_test(test_reversi test_reversi)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS test_board test_minimax test_reversi)
