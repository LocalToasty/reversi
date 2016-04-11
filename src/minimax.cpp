#include "minimax.hpp"
#include <chrono>
#include <cmath>
#include <tuple>
#include "board.hpp"

namespace ch = std::chrono;
using boost::optional;
using std::size_t;

// declarations
double composite_heuristic(Board const& board, Player player);
double corners_captured(Board const& board, Player player);
double stability(Board const& board, Player player);
double disk_parity(Board const& board, Player player);
double static_heuristic(Board const& board, Player player);
double mobility(Board const& board, Player player);

//! Calculates the maximum reachable value of a board configuration.
template <typename Heuristic>
double minimax_depth(Board const& board, Player player, size_t depth,
                     double alpha, double beta, Heuristic heuristic) {
  if (depth == 0 || board.game_over()) {
    // maximum iteration depth or final board state reached
    return heuristic(board, player);
  }

  double best_value = alpha;

  for (auto next : board.next_boards(player)) {
    Move move;
    Board next_board;
    std::tie(move, next_board) = next;

    Player opponent = (player == Disk::dark) ? Disk::light : Disk::dark;

    double value = (!next_board.legal_moves(opponent).empty())
                       ? -minimax_depth(next_board, opponent, depth - 1, -beta,
                                        -alpha, heuristic)
                       : minimax_depth(next_board, player, depth - 1, alpha,
                                       beta, heuristic);

    if (value > best_value) {
      best_value = value;
    }

    if (value > alpha) {
      alpha = value;
    }

    if (beta <= alpha) {
      // beta cut off
      return best_value;
    }
  }

  return best_value;
}

/*! Determines move using the minimax algorithm.
 *
 * \param heuristic  a function rating the value of a board. The heuristic is
 * expected to return a value between -1 and 1, where -1 is the worst and 1 the
 * best possible result.
 *
 * \returns the best move found.
 */
template <typename Heuristic>
Move generic_minimax_actor(Board const& board, Player player,
                           optional<duration> budget, Heuristic heuristic) {
  if (board.disk_no() == 4) {
    // for the first turn, all possible moves are the same
    return board.legal_moves(player)[0];
  }

  // time when the computation started
  auto start_time = ch::steady_clock::now();

  // best move found so far
  Move best_move = {-1, -1};

  // expected average branching factor
  double constexpr branch_fac = 2;

  // time of the last iteration
  auto last_it_duration = ch::seconds(1) / branch_fac;

  size_t depth = 1;
  size_t const max_remaining_moves = board.size * board.size - board.disk_no();

  // time when the computation needs to be finished
  auto end_time =
      start_time +
      (budget ? ch::duration_cast<duration>(
                    *budget / std::ceil((double)max_remaining_moves / 2))
              : ch::duration_cast<duration>(ch::seconds(30)));

  // iterative deepening
  while (depth == 1 ||  // always do at least one iteration
         (end_time - ch::steady_clock::now() > branch_fac * last_it_duration &&
          end_time > ch::steady_clock::now() &&  // guard against overflow
          depth <= max_remaining_moves)) {
    auto iteration_start_time = ch::steady_clock::now();

    // values for alpha-beta cutoff
    double alpha = -1;
    double beta = 1;

    double best_value = alpha;

    // recursively do minimax search on all possible next boards
    for (auto next : board.next_boards(player)) {
      Move move;
      Board next_board;
      std::tie(move, next_board) = next;

      Player opponent = (player == Disk::dark) ? Disk::light : Disk::dark;

      double value = (!next_board.legal_moves(opponent).empty())
                         ? -minimax_depth(next_board, opponent, depth - 1,
                                          -beta, -alpha, heuristic)
                         : minimax_depth(next_board, player, depth - 1, alpha,
                                         beta, heuristic);

      if (value > best_value) {
        best_value = value;
        best_move = move;
      }

      if (value > alpha) {
        alpha = value;
      }
    }

    depth++;
    last_it_duration = ch::steady_clock::now() - iteration_start_time;
  }

  return best_move;
}

/*! The default minimax actor.
 * \see generic_minimax_actor
 */
Move minimax_actor(Board const& board, Player player,
                   optional<duration> budget) {
  return generic_minimax_actor(board, player, budget, composite_heuristic);
}

/*! Rates a board.
 *
 * \returnsa value in the interval [-1, 1], where -1 is the worst and 1 is the
 * best possible rating of the board for the player.
 */
double composite_heuristic(Board const& board, Player player) {
  if (board.disk_no() > board.size * board.size || board.game_over()) {
    return disk_parity(board, player);
  } else {
    return (6 * corners_captured(board, player) + 5 * stability(board, player) +
            1 * disk_parity(board, player) +
            5 * static_heuristic(board, player) + 1 * mobility(board, player)) /
           (6 + 5 + 1 + 5 + 1);
  }
}

//! Calculates the relative amount of corners captured by a player.
double corners_captured(Board const& board, Player player) {
  using Pos = std::pair<size_t, size_t>;
  double corner_diff = 0;
  double corners_captured = 0;

  std::vector<Pos> const corners = {{0, 0},
                                    {board.size - 1, board.size - 1},
                                    {0, board.size - 1},
                                    {board.size - 1, 0}};
  for (auto corner : corners) {
    size_t x, y;
    std::tie(x, y) = corner;
    corner_diff += board[x][y];
    if (board[x][y] != Disk::none) {
      corners_captured++;
    }
  }

  if (corners_captured) {
    return player * corner_diff / corners_captured;
  } else {
    return 0;
  }
}

/*! Finds all semi-stable disks.
 *
 * A disk is semi-stable, if it can not be flipped within one turn.
 */
std::array<std::array<bool, Board::size>, Board::size> semi_stable_disks(
    Board const& board) {
  std::array<std::array<bool, Board::size>, Board::size> semi_stable;

  // initialize all disks as stable
  for (size_t x = 0; x < board.size; x++) {
    for (size_t y = 0; y < board.size; y++) {
      semi_stable[x][y] = (board[x][y] != Disk::none);
    }
  }

  // chec which disks can be flipped by executing all possible moves
  for (Player player : {Player::dark, Player::light}) {
    for (auto next : board.next_boards(player)) {
      Board const& next_board = next.second;
      for (size_t x = 0; x < board.size; x++) {
        for (size_t y = 0; y < board.size; y++) {
          if (next_board[x][y] != board[x][y] && board[x][y] != Disk::none) {
            semi_stable[x][y] = false;
          }
        }
      }
    }
  }

  return semi_stable;
}

/*! Check if a disk is in a full row, column and diagonal.
 *
 * If these conditions are true, the disk is guaranteed to be stable.
 */
bool in_full_row(Board const& board, size_t x, size_t y) {
  // row / col
  for (size_t i = 0; i < board.size; i++) {
    if (board[i][y] == Disk::none || board[x][i] == Disk::none) {
      // row / col is not full
      return false;
    }
  }

  // diagonal from top-left to bottom-right
  int i = 0;
  int j = 0;
  if (x > y) {
    i = x - y;
  } else {
    j = y - x;
  }

  while ((size_t)i < board.size && (size_t)j < board.size) {
    if (board[i][j] == Disk::none) {
      // diagonal is not full
      return false;
    }
    i++;
    j++;
  }

  // diagonal from bottom-left to top-right
  i = board.size - 1;
  j = 0;
  if (board.size - x - 1 > y) {
    i = x + y;
  } else {
    j = y - (board.size - x - 1);
  }

  while (i >= 0 && (size_t)j < board.size) {
    if (board[i][j] == Disk::none) {
      // diagonal is not full
      return false;
    }

    i--;
    j++;
  }

  // all rows are full
  return true;
}

//! Checks if the given coordinates are outside of the board.
bool is_edge(Board const& board, int x, int y) {
  return x < 0 || (size_t)x >= board.size || y < 0 || (size_t)y >= board.size;
}

/*! Checks if all neighbors of a disk are stable.
 *
 * The stability is only checked in the context of the passed stable array.
 */
bool neighbours_stable(
    Board const& board,
    std::array<std::array<bool, Board::size>, Board::size> const& stable,
    size_t x, size_t y, Player player) {
  std::vector<Move> directions{{-1, -1}, {0, -1}, {1, -1}, {1, 0}};
  for (Move move : directions) {
    size_t dx, dy;
    std::tie(dx, dy) = move;
    if (!(is_edge(board, x + dx, y + dy) || is_edge(board, x - dx, y - dy) ||
          (stable[x + dx][y + dy] && board[x + dx][y + dy] == player) ||
          (stable[x - dx][y - dy] && board[x - dx][y - dy] == player))) {
      return false;
    }
  }

  return true;
}

/*! Determines all stable disks.
 *
 * A disk is considered stable, if it cannot to be flipped any more.
 */
std::array<std::array<bool, Board::size>, Board::size> stable_disks(
    Board const& board) {
  std::array<std::array<bool, Board::size>, Board::size> stable;

  // all disks which are in a full row, column and diagonals are guaranteed to
  // be stable
  for (size_t x = 0; x < board.size; x++) {
    for (size_t y = 0; y < board.size; y++) {
      stable[x][y] = in_full_row(board, x, y);
    }
  }

  // try to find new stable disks by checking if the neighbors are stable
  bool change_made;
  do {
    change_made = false;

    for (size_t x = 0; x < board.size; x++) {
      for (size_t y = 0; y < board.size; y++) {
        if (!stable[x][y] and board[x][y] != Disk::none) {
          if (neighbours_stable(board, stable, x, y, board[x][y])) {
            stable[x][y] = true;
            change_made = true;
          }
        }
      }
    }
  } while (change_made);

  return stable;
}

//! Determines which player has the stability advantage.
double stability(Board const& board, Player player) {
  double dark_score = 0;
  double light_score = 0;

  auto stable = stable_disks(board);
  auto semi_stable = semi_stable_disks(board);

  for (size_t x = 0; x < board.size; x++) {
    for (size_t y = 0; y < board.size; y++) {
      switch (board[x][y]) {
        case Disk::dark:
          if (stable[x][y]) {
            dark_score += 1;
          } else if (!semi_stable[x][y]) {
            dark_score -= 1;
          }
          break;

        case Disk::light:
          if (stable[x][y]) {
            light_score += 1;
          } else if (!semi_stable[x][y]) {
            light_score -= 1;
          }
          break;

        default:
          break;
      }
    }
  }

  double score_sum = std::abs(dark_score) + std::abs(light_score);
  if (score_sum) {
    return player * (dark_score - light_score) / score_sum;
  } else {
    return 0;
  }
}

//! Calculates the relative amount of disks a player has.
double disk_parity(Board const& board, Player player) {
  double disk_diff = 0;
  for (size_t x = 0; x < board.size; x++) {
    for (size_t y = 0; y < board.size; y++) {
      disk_diff += board[x][y];
    }
  }

  return player * disk_diff / board.disk_no();
}

//! Rates the captured disks based on static disk values.
double static_heuristic(Board const& board, Player player) {
  double value[board.size][board.size] = {
      {+4, -3, +2, +2, +2, +2, -3, +4}, {-3, -4, -1, -1, -1, -1, -4, -3},
      {+2, -1, +1, +0, +0, +1, -1, +2}, {+2, -1, +0, +1, +1, +0, -1, +2},
      {+2, -1, +0, +1, +1, +0, -1, +2}, {+2, -1, +1, +0, +0, +1, -1, +2},
      {-3, -4, -1, -1, -1, -1, -4, -3}, {+4, -3, +2, +2, +2, +2, -3, +4}};

  double dark_score = 0;
  double light_score = 0;

  for (size_t x = 0; x < board.size; x++) {
    for (size_t y = 0; y < board.size; y++) {
      switch (board[x][y]) {
        case Disk::dark:
          dark_score += value[x][y];
          break;

        case Disk::light:
          light_score += value[x][y];
          break;

        default:
          break;
      }
    }
  }

  double score_sum = std::abs(dark_score) + std::abs(light_score);
  if (score_sum) {
    return player * (dark_score - light_score) / score_sum;
  } else {
    return 0;
  }
}

//! Checks which player has the mobility advantage.
double mobility(Board const& board, Player player) {
  double dark_mobility = board.legal_moves(Player::dark).size();
  double light_mobility = board.legal_moves(Player::light).size();

  if (dark_mobility + light_mobility) {
    return player * (dark_mobility - light_mobility) /
           (dark_mobility + light_mobility);
  } else {
    return 0;
  }
}
