#ifndef REVERSI_MINIMAX_H_
#define REVERSI_MINIMAX_H_

#include <chrono>
#include "board.hpp"
#include "reversi.hpp"

Move minimax_actor(Board const& board, Player player,
                   boost::optional<duration> budget);

#endif
