#include <chrono>
#include <iostream>
#include "human.hpp"
#include "minimax.hpp"
#include "reversi.hpp"

int main() {
  // play a game of reversi
  Player winner = play_reversi(
      human_actor,
      std::chrono::duration_cast<duration>(std::chrono::minutes(30)),
      minimax_actor,
      std::chrono::duration_cast<duration>(std::chrono::minutes(30)), false);

  // announce winner
  switch (winner) {
    case Player::dark:
      std::cout << "Dark wins the game" << std::endl;
      break;

    case Player::light:
      std::cout << "Light wins the game" << std::endl;
      break;

    default:
      std::cout << "It's a draw" << std::endl;
      break;
  }
}
