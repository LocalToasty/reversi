#ifndef REVERSI_REVERSI_H_
#define REVERSI_REVERSI_H_

#include <functional>
#include "board.hpp"

Disk play_reversi(std::function<Move(Board const&, Player)> dark_actor,
                  std::function<Move(Board const&, Player)> light_actor,
                  bool verbose = false);

#endif
