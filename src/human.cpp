#include "human.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include "board.hpp"
#include "reversi.hpp"

namespace ch = std::chrono;
using boost::optional;

//! Reads a move from the user.
optional<Move> read_move() {
  char col;
  int row;
  std::cin >> col >> row;

  if (!std::cin) {
    // an error has occured while parsing
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return boost::none;
  }

  if (col >= 'a' && col <= 'h' && row >= 1 && row <= 8) {
    // valid move format
    return std::make_pair((size_t)(col - 'a'), (size_t)(row - 1));
  } else {
    return boost::none;
  }
}

Move human_actor(Board const& board, Player player, optional<duration> budget) {
  auto start_time = ch::steady_clock::now();

  // print player
  switch (player) {
    case Player::dark:
      std::cout << "It's dark's turn." << std::endl;
      break;

    case Player::light:
      std::cout << "It's light's turn." << std::endl;
      break;

    default:
      // TODO throw
      break;
  }

  // get a valid move
  while (true) {
    if (budget) {
      // print remaining time
      std::cout << "You have "
                << ch::duration_cast<ch::seconds>(
                       *budget - (ch::steady_clock::now() - start_time))
                       .count()
                << " seconds left." << std::endl;
    }

    std::cout << "Current board:" << board << "Enter your move: ";

    if (auto move = read_move()) {
      // move could be parsed
      if (auto next = board.next_board(*move, player)) {
        // move is legal
        std::cout << "The resulting board would look like this:" << *next
                  << "continue? [y/N] ";

        // confirm move
        char answer;
        std::cin >> answer;
        switch (answer) {
          case 'y':
          case 'Y':
            return *move;
            break;

          default:
            break;
        }
      } else {
        // illegal move
        std::cout << "Illegal move." << std::endl;
      }
    } else {
      // input could not be parsed
      std::cout << "Invalid input format. The move has to be entered in the "
                   "format e5."
                << std::endl;
    }
  }
}
