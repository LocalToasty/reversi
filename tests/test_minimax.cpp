#define BOOST_TEST_MODULE test_minimax
#include <cstdlib>
#include <boost/test/included/unit_test.hpp>
#include "minimax.hpp"
#include "board.hpp"

double composite_heuristic(Board const& board, Player player);

BOOST_AUTO_TEST_CASE(test_composite_heuristic) {
  Board board;

  Player player = Player::dark;

  // play a random game of reversi; check composite_heuristic for each board
  while (!board.game_over()) {
    double heuristic_dark = composite_heuristic(board, Player::dark);
    double heuristic_light = composite_heuristic(board, Player::light);

    // assert that composite_heuristic stays within bounds
    BOOST_TEST(heuristic_dark >= -1);
    BOOST_TEST(heuristic_dark <= 1);

    BOOST_TEST(heuristic_dark == -heuristic_light);

    Player opponent = (player == Disk::dark) ? Disk::light : Disk::dark;
    auto moves = board.legal_moves(player);
    if (!moves.empty()) {
      board = *board.next_board(moves[rand() % moves.size()], player);
      player = opponent;
    } else {
      moves = board.legal_moves(opponent);
      if (!moves.empty()) {
        board = *board.next_board(moves[rand() % moves.size()], opponent);
      }
    }
  }
}
