#include "board.hpp"
#include <tuple>
#include <boost/optional.hpp>

Board::Board() {
  // set up initial disks
  for (size_t x = 0; x < size; x++) {
    _squares[x].fill(Disk::none);
  }
  _squares[size / 2 - 1][size / 2 - 1] = Disk::light;
  _squares[size / 2][size / 2] = Disk::light;
  _squares[size / 2][size / 2 - 1] = Disk::dark;
  _squares[size / 2 - 1][size / 2] = Disk::dark;
}

std::array<Disk, Board::size>& Board::operator[](std::size_t index) {
  return _squares[index];
}

std::array<Disk, Board::size> const& Board::operator[](
    std::size_t index) const {
  return _squares[index];
}

bool Board::legal_move(Move move, Player player) const {
  int x, y;
  std::tie(x, y) = move;

  if (_squares[x][y] != Disk::none) {
    // if the square is occupied, the move is illegal
    return false;
  }

  // check if any enemy disks would be flipped by this action
  for (int dx : {-1, 0, 1}) {
    for (int dy : {-1, 0, 1}) {
      if (dx == 0 && dy == 0) {
        continue;
      }

      // an opponent has been found in this direction
      bool opp_found = false;

      // coordinates of the currently examined tile
      int px = x + dx;
      int py = y + dy;

      while (px >= 0 && px < size && py >= 0 && py < size) {
        if (_squares[px][py] == Disk::none) {
          // an empty space is interrupting the chain
          // there are no disks to be flipped here
          break;
        } else if (_squares[px][py] != player) {
          // an opponents disk to be flipped has been found
          opp_found = true;
        } else if (opp_found) {
          // there is an uninterrupted row of enemy disks in between two of
          // the players disks, meaning the move is legal
          return true;
        } else {
          // there are no enemy disks to be captured;
          // try in another direction
          break;
        }

        px += dx;
        py += dy;
      }
    }
  }

  // the move is not valid
  return false;
}

std::vector<Move> Board::legal_moves(Player player) const {
  std::vector<Move> moves;

  for (std::size_t y = 0; y < size; y++) {
    for (std::size_t x = 0; x < size; x++) {
      if (legal_move({x, y}, player)) moves.push_back({x, y});
    }
  }

  return moves;
}

bool Board::game_over() const {
  // the game is over if no player can do a legal move
  return legal_moves(Disk::dark).empty() && legal_moves(Disk::light).empty();
}

size_t Board::disk_no() const {
  size_t no = 0;

  for (auto col : _squares) {
    for (Disk disk : col) {
      if (disk != Disk::none) {
        no++;
      }
    }
  }

  return no;
}

std::vector<std::pair<Move, Board>> Board::next_boards(Player player) const {
  std::vector<std::pair<Move, Board>> boards;

  for (size_t x = 0; x < size; x++) {
    for (size_t y = 0; y < size; y++) {
      Move move = {x, y};
      if (boost::optional<Board> board = next_board(move, player)) {
        boards.push_back({move, *board});
      }
    }
  }

  return boards;
}

boost::optional<Board> Board::next_board(Move move, Player player) const {
  int x, y;
  std::tie(x, y) = move;

  if (_squares[x][y] != Disk::none) {
    // if the square is occupied, the move is illegal
    return boost::none;
  }

  Board next(*this);
  next[x][y] = player;

  bool change_made = false;

  // check if any enemy disks would be flipped by this action
  for (int dx : {-1, 0, 1}) {
    for (int dy : {-1, 0, 1}) {
      if (dx == 0 && dy == 0) {
        continue;
      }

      // an opponent has been found in this direction
      bool opp_found = false;

      // coordinates of the currently examined tile
      int px = x + dx;
      int py = y + dy;

      while (px >= 0 && px < size && py >= 0 && py < size) {
        if (_squares[px][py] == Disk::none) {
          // an empty space is interrupting the chain
          // there are no disks to be flipped here
          break;
        } else if (_squares[px][py] != player) {
          // an opponents disk to be flipped has been found
          opp_found = true;
        } else if (opp_found) {
          // there is an uninterrupted row of enemy disks in between two of
          // the players disks, meaning the move is legal
          change_made = true;

          // flip disks
          while (px != x || py != y) {
            next[px][py] = player;

            px -= dx;
            py -= dy;
          }

          break;
        } else {
          // there are no enemy disks to be captured;
          // try in another direction
          break;
        }

        px += dx;
        py += dy;
      }
    }
  }

  if (change_made) {
    return next;
  } else {
    return boost::none;
  }
}

std::array<std::array<Disk, Board::size>, Board::size>::iterator
Board::begin() {
  return _squares.begin();
}

std::array<std::array<Disk, Board::size>, Board::size>::iterator Board::end() {
  return _squares.end();
}

std::ostream& operator<<(std::ostream& out, Board const& board) {
  // print column descriptors
  out << std::endl << ' ';
  for (int col = 0; col < board.size; col++) {
    out << (char)('a' + col);
  }
  out << std::endl;

  for (int row = 0; row < board.size; row++) {
    out << row + 1;

    for (int col = 0; col < board.size; col++) {
      switch (board[col][row]) {
        case Disk::none:
          out << '.';
          break;

        case Disk::dark:
          out << 'x';
          break;

        case Disk::light:
          out << 'o';
          break;
      }
    }

    out << std::endl;
  }

  return out;
}
