#ifndef REVERSI_REVERSI_H_
#define REVERSI_REVERSI_H_

#include <chrono>
#include <functional>
#include "board.hpp"

using duration = std::chrono::duration<double>;
using Actor =
    std::function<Move(Board const&, Player, boost::optional<duration>)>;

Player play_reversi(Actor dark_actor,
                    boost::optional<duration> dark_time_budget,
                    Actor light_actor,
                    boost::optional<duration> light_time_budget,
                    bool verbose = false);

#endif
