#ifndef REVERSI_HUMAN_H_
#define REVERSI_HUMAN_H_

#include "board.hpp"
#include "reversi.hpp"

Move human_actor(Board const& board, Player player,
                 boost::optional<duration> budget);

#endif
