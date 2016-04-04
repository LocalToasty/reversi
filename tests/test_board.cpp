#define BOOST_TEST_MODULE test_board
#include <boost/test/included/unit_test.hpp>
#include "board.hpp"

BOOST_AUTO_TEST_CASE(test_legal_move) {
  Board board;

  BOOST_TEST(!board.legal_move({0, 0}, Disk::dark));

  BOOST_TEST(!board.legal_move({4, 2}, Disk::dark));
  BOOST_TEST(!board.legal_move({3, 2}, Disk::light));

  BOOST_TEST(board.legal_move({3, 2}, Disk::dark));
  BOOST_TEST(board.legal_move({4, 2}, Disk::light));
}

BOOST_AUTO_TEST_CASE(test_next_board) {
  Board board;

  BOOST_TEST(!board.next_board({3, 2}, Disk::light));

  Board next = *board.next_board({3, 2}, Disk::dark);
  BOOST_TEST(next[3][2] == Disk::dark);
  BOOST_TEST(next[3][3] == Disk::dark);
}
