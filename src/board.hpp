#ifndef REVERSI_BOARD_H_
#define REVERSI_BOARD_H_

#include <array>
#include <cstdint>
#include <ostream>
#include <vector>
#include <boost/optional.hpp>

enum Disk { none = 0, dark = 1, light = -1 };

using Player = Disk;

using Move = std::pair<std::size_t, std::size_t>;

/*! Reversi Board.
 *
 * The reversi board is assumed to be 8*8 squares big.
 */
class Board {
 public:
  //! The length of the board.
  std::size_t static constexpr size = 8;

  //! Create a new board.
  Board();

  std::array<Disk, size>& operator[](std::size_t index);
  std::array<Disk, size> const& operator[](std::size_t index) const;

  //! Check if a move is legal.
  bool legal_move(Move move, Player player) const;

  //! Determines all possible moves.
  std::vector<Move> legal_moves(Player player) const;

  boost::optional<Board> next_board(Move move, Player player) const;
  std::vector<std::pair<Move, Board>> next_boards(Player player) const;

  //! Determine if the board is in a final position.
  bool game_over() const;

  //! Number of disks.
  std::size_t disk_no() const;

  std::array<std::array<Disk, size>, size>::iterator begin();
  std::array<std::array<Disk, size>, size>::iterator end();

 private:
  /* The internal representation of the board contents.
   *
   * The outer array holds the individual columns of the board.
   */
  std::array<std::array<Disk, size>, size> _squares;
};

std::ostream& operator<<(std::ostream& out, Board const& board);

#endif
