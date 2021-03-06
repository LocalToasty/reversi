#define BOOST_TEST_MODULE test_board
#include <boost/test/included/unit_test.hpp>
#include "reversi.hpp"
#include "board.hpp"

Move simple_actor(Board const& board, Player player) {
  return board.legal_moves(player)[0];
}

BOOST_AUTO_TEST_CASE(test_reversi) {
  Board board;

  BOOST_TEST(play_reversi(simple_actor, simple_actor, true) == Disk::light);
}
