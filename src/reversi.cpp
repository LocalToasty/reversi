#include "reversi.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include "board.hpp"

using boost::optional;

std::ostream& operator<<(std::ostream& out, Board const& board) {
  // print column descriptors
  out << std::endl << ' ';
  for (int col = 0; col < board.size; col++) {
    out << (char)('a' + col);
  }
  out << std::endl;

  for (int row = 0; row < board.size; row++) {
    out << row + 1;

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

/*! Plays a game of reversi.
 *
 * The time budgets describe what amount of time each actor has in total to do
 * his actions.
 * If it is set to none, no time limit is imposed.
 * If an actor misses his deadline, he automatically looses the game.
 *
 * \returns the winner of the game.
 */
Player play_reversi(Actor dark_actor, optional<duration> dark_time_budget,
                    Actor light_actor, optional<duration> light_time_budget,
                    bool verbose) {
  Board board;

  if (verbose) {
    std::cout << board;
  }

  Player player = Player::dark;  // the current player

  while (true) {
    // get the players move
    Move move = {0, 0};
    switch (player) {
      case Player::dark: {
        auto start_time = std::chrono::steady_clock::now();
        move = dark_actor(board, Player::dark, dark_time_budget);
        auto end_time = std::chrono::steady_clock::now();

        if (dark_time_budget) {
          auto needed_time =
              std::chrono::duration_cast<duration>(end_time - start_time);

          if (needed_time > dark_time_budget) {
            // time budget expended; default win for light
            return Player::light;
          }

          *dark_time_budget -= needed_time;

          if (verbose) {
            std::cout << "remaining time: "
                      << std::chrono::duration_cast<std::chrono::seconds>(
                             *dark_time_budget)
                             .count()
                      << " s";
          }
        }

        break;
      }

      case Player::light: {
        auto start_time = std::chrono::steady_clock::now();
        move = light_actor(board, Player::light, light_time_budget);
        auto end_time = std::chrono::steady_clock::now();

        if (light_time_budget) {
          auto needed_time =
              std::chrono::duration_cast<duration>(end_time - start_time);

          if (needed_time > light_time_budget) {
            // time budget expended; default win for light
            return Player::dark;
          }

          *light_time_budget -= needed_time;

          if (verbose) {
            std::cout << "remaining time: "
                      << std::chrono::duration_cast<std::chrono::seconds>(
                             *light_time_budget)
                             .count()
                      << " s";
          }
        }
        break;
      }

      default:
        // TODO throw
        break;
    }

    Player opponent = (player == Player::dark) ? Player::light : Player::dark;
    if (optional<Board> next_board = board.next_board(move, player)) {
      board = *next_board;
    } else {
      // The actor did an invalid move; the opponent wins by default
      return opponent;
    }

    if (verbose) {
      std::cout << board;
    }

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
