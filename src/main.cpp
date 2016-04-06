#include <chrono>
#include "minimax.hpp"
#include "reversi.hpp"

int main() {
  play_reversi(minimax_actor,
               std::chrono::duration_cast<duration>(std::chrono::minutes(5)),
               minimax_actor,
               std::chrono::duration_cast<duration>(std::chrono::minutes(5)),
               true);
}
