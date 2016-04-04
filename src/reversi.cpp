#include <functional>
#include <iostream>
#include "board.hpp"

std::ostream& operator<<(std::ostream& out, Board const& board) {
  // print column descriptors
  out << std::endl << ' ';
  for (int col = 0; col < board.size; col++) {
    out << (char)('a' + col);
  }
  out << std::endl;

  for (int row = 0; row < board.size; row++) {
    out << row;

    for (int col = 0; col < board.size; col++) {
      switch (board[col][row]) {
        case Disk::none:
          out << '.';
          break;

        case Disk::dark:
          out << 'x';
          break;

        case Disk::light:
          out << 'o';
          break;
      }
    }

    out << std::endl;
  }

  return out;
}

Disk play_reversi(std::function<Move(Board const&, Player)> dark_actor,
                  std::function<Move(Board const&, Player)> light_actor,
                  bool verbose) {
  Board board;

  if (verbose) {
    std::cout << board;
  }

  Player player = Player::dark;  // the current player

  while (true) {
    // get the players move
    Move move = (player == Player::dark) ? dark_actor(board, Player::dark)
                                         : light_actor(board, Player::light);

    if (boost::optional<Board> next_board = board.next_board(move, player)) {
      board = *next_board;
    } else {
      // TODO throw
    }

    if (verbose) {
      std::cout << board;
    }

    Player opponent = (player == Player::dark) ? Player::light : Player::dark;
    if (!board.legal_moves(opponent).empty()) {
      // the other player has to do a move
      player = opponent;
    } else if (board.legal_moves(player).empty()) {
      // no player can do a move, the game is over

      // calculate disk difference
      int disk_diff = 0;
      for (auto col : board) {
        for (auto square : col) {
          disk_diff += square;
        }
      }

      // the player with more disks wins
      if (disk_diff > 0) {
        return Player::dark;
      } else if (disk_diff < 0) {
        return Player::light;
      } else {
        return Player::none;
      }
    }
  }
}
