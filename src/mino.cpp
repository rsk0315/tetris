#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <tuple>
#include <utility>

class game;
class board;
class mino;

class board {
  friend game;
  static size_t constexpr S_rows = 20;
  static size_t constexpr S_columns = 10;
  static size_t constexpr S_margin = 3;
  using board_type = std::array<std::array<int, S_columns+2*S_margin>, S_rows+2*S_margin>;

  board_type M_board = {};

public:
  board() = default;
  board(board const&) = default;
  board(board&&) = default;

  bool occupied(size_t i, size_t j) const {
    return M_board[S_margin+i][S_margin+j];
  }
};

class mino {
  friend game;
  using block_type = std::array<std::array<int, 4>, 4>;

  char M_type;
  block_type M_block;
  int M_state = 0;

  size_t M_i = 0, M_j = 5;

  void M_block_init() {
    switch (M_type) {
    case 'I':
      M_block = block_type{ {{0, 0, 0, 0},
                             {1, 1, 1, 1},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case 'J':
      M_block = block_type{ {{1, 0, 0, 0},
                             {1, 1, 1, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case 'L':
      M_block = block_type{ {{0, 0, 1, 0},
                             {1, 1, 1, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case '0':
      M_block = block_type{ {{0, 1, 1, 0},
                             {0, 1, 1, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case 'S':
      M_block = block_type{ {{0, 1, 1, 0},
                             {1, 1, 0, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case 'T':
      M_block = block_type{ {{0, 1, 0, 0},
                             {1, 1, 1, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    case 'Z':
      M_block = block_type{ {{1, 1, 0, 0},
                             {0, 1, 1, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}} }; break;
    default:
      throw std::invalid_argument("invalid mino-type");
    }
  }

  std::array<std::pair<ptrdiff_t, ptrdiff_t>, 5> M_offset_array(bool clockwise) const {
    if (M_type == 'I') {
      if (clockwise)  {
        switch (M_state) {
        case 0:
          return {{{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}};
        case 1:
          return {{{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}};
        case 2:
          return {{{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}};
        case 3:
          return {{{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}};
        }
      } else {
        switch (M_state) {
        case 0:
          return {{{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}};
        case 1:
          return {{{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}};
        case 2:
          return {{{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}};
        case 3:
          return {{{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}};
        }
      }
    } else {
      if (clockwise) {
        switch (M_state) {
        case 0:
          return {{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}};
        case 1:
          return {{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}};
        case 2:
          return {{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}};
        case 3:
          return {{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}};
        }
      } else {
        switch (M_state) {
        case 0:
          return {{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}};
        case 1:
          return {{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}};
        case 2:
          return {{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}};
        case 3:
          return {{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}};
        }
      }
    }
    assert(false);
  }

public:
  mino() = default;
  mino(mino const&) = default;
  mino(mino&&) = default;
  mino(char type): M_type(type) { M_block_init(); }

  bool move(ptrdiff_t di, ptrdiff_t dj, board const& base) {
    M_i += di;
    M_j += dj;
    return true;  // XXX
  }

  std::pair<ptrdiff_t, ptrdiff_t> rotate(bool clockwise, board const& base) {
    if (M_type == 'O') return {0, 0};

    block_type next_block = M_block;
    {
      /* rotate */
      size_t frame = ((M_type == 'I')? 4:3);

      auto tmp = M_block;
      for (auto& bi: M_block)
        for (auto& bij: bi) bij = 0;
      if (clockwise) {
        for (size_t i = 0; i < frame; ++i)
          for (size_t j = 0; j < frame; ++j)
            next_block[j][frame-1-i] = tmp[i][j];
      } else {
        for (size_t i = 0; i < frame; ++i)
          for (size_t j = 0; j < frame; ++j)
            next_block[frame-1-j][i] = tmp[i][j];
      }
    }

    int next_state = M_state + (clockwise? +1:-1);
    {
      /* check */
      for (auto const& offset: M_offset_array(clockwise)) {
        ptrdiff_t i0, j0;
        std::tie(i0, j0) = offset;
        bool valid = true;
        for (size_t i = 0; i < 4; ++i)
          for (size_t j = 0; j < 4; ++j) {
            if (!next_block[i][j]) continue;
            if (base.occupied(M_i+i+i0, M_j+j+j0)) {
              valid = false;
              break;
            }
          }

        if (valid) {
          M_state = next_state;
          M_block = next_block;
          return offset;
        }
      }
    }

    throw std::logic_error("cannot rotate");
  }

  bool occupies(size_t i, size_t j) const {
    if (!(i-M_i < 4 && j-M_j < 4)) return false;
    return M_block[i-M_i][j-M_j];
  }

  void inspect() const {
    for (size_t i = 0; i < 4; ++i)
      for (size_t j = 0; j < 4; ++j)
        fprintf(stderr, "%c%c", M_block[i][j]? '#':'.', j<3? ' ':'\n');
  }
};

class game {
  board b;
  
  void inspect(board const& b, mino const& m) const {
    for (size_t i = 0; i < b.S_rows; ++i)
      for (size_t j = 0; j < b.S_columns; ++j) {
        char cur = '.';
        if (b.M_board[i+b.S_margin][j+b.S_margin]) cur = '#';
        if (m.occupies(i, j)) cur = '=';
        fprintf(stderr, "%c%c", cur, j+1<b.S_columns? ' ':'\n');
      }
  }

public:
  game() = default;

  void play() {
    mino i('I');
    inspect(b, i);
  }
};

int main() {
  game g;
  g.play();
}
