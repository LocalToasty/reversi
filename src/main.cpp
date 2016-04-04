#include <chrono>
#include "minimax.hpp"
#include "reversi.hpp"

int main() {
  play_reversi(
      minimax_actor, boost::none,
      // std::chrono::duration_cast<duration>(std::chrono::minutes(30)),
      minimax_actor,
      std::chrono::duration_cast<duration>(std::chrono::minutes(30)), true);
}
