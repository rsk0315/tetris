#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cctype>
#include <cassert>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <string>
#include <random>

class game;
class board;
class mino;

class board {
  friend game;
  static size_t constexpr S_rows = 21;
  static size_t constexpr S_columns = 10;
  static size_t constexpr S_margin = 3;
  using board_type = std::array<std::array<int, S_columns+2*S_margin>, S_rows+2*S_margin>;

  board_type M_board = {};

public:
  board() = default;
  board(board const&) = default;
  board(board&&) = default;

  bool out_of_range(size_t i, size_t j) const {
    return (!(i < S_rows && j < S_columns));
  }

  bool occupied(size_t i, size_t j) const {
    return M_board[i+S_margin][j+S_margin];
  }

  void set(size_t i, size_t j, char) {
    M_board[i+S_margin][j+S_margin] = 1;
  }

  size_t erase() {
    size_t res = 0;
    // XXX undesired auxiliary array
    auto tmp = M_board;
    size_t k = S_rows-1 + S_margin;
    for (size_t i = S_rows; i--;) {
      bool erasing = true;
      for (size_t j = 0; j < S_columns; ++j) {
        if (!occupied(i, j)) {
          erasing = false;
          break;
        }
      }
      if (erasing) {
        ++res;
      } else {
        tmp[k--] = M_board[i+S_margin];
      }
    }
    M_board = tmp;
    return res;
  }
};

class mino {
  friend game;
  using block_type = std::array<std::array<int, 4>, 4>;

  char M_type;
  block_type M_block;
  int M_state = 0;

  size_t M_i = 0, M_j = 3;

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
    case 'O':
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
        case 1:
          return {{{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}};
        case 2:
          return {{{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}};
        case 3:
          return {{{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}};
        case 0:
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
        case 1:
          return {{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}};
        case 2:
          return {{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}};
        case 3:
          return {{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}};
        case 0:
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
    size_t next_i = M_i + di;
    size_t next_j = M_j + dj;
    {
      size_t frame = 4;
      for (size_t i = 0; i < frame; ++i)
        for (size_t j = 0; j < frame; ++j) {
          if (!M_block[i][j]) continue;
          if (base.out_of_range(next_i+i, next_j+j)) return false;
          if (base.occupied(next_i+i, next_j+j)) return false;
        }
    }
    M_i = next_i;
    M_j = next_j;
    return true;
  }

  bool rotate(bool clockwise, board const& base) {
    if (M_type == 'O') return true;

    block_type next_block = {};
    {
      /* rotate */
      size_t frame = ((M_type == 'I')? 4:3);

      auto tmp = M_block;
      // for (auto& bi: M_block)
      //   for (auto& bij: bi) bij = 0;
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
    (next_state += 4) %= 4;
    {
      /* check */
      for (auto const& offset: M_offset_array(clockwise)) {
        ptrdiff_t i0, j0;
        std::tie(j0, i0) = offset;
        i0 = -i0;
        bool valid = true;
        for (size_t i = 0; i < 4; ++i)
          for (size_t j = 0; j < 4; ++j) {
            if (!next_block[i][j]) continue;
            if (base.out_of_range(M_i+i+i0, M_j+j+j0) || base.occupied(M_i+i+i0, M_j+j+j0)) {
              valid = false;
              break;
            }
          }

        if (valid) {
          M_state = next_state;
          M_block = next_block;
          M_i += i0;
          M_j += j0;
          return true;
        }
      }
    }

    return false;
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
  std::string const S_minos = "ILJOSZT";
  std::string M_minos = S_minos;
  std::mt19937 M_rng;
  char M_hold = 0;

  void M_inspect(board const& b) const {
    if (M_hold != 0) fprintf(stderr, "holding %c-mino\n", M_hold);
    for (size_t i = 0; i < b.S_rows; ++i)
      for (size_t j = 0; j < b.S_columns; ++j) {
        char cur = '.';
        if (b.M_board[i+b.S_margin][j+b.S_margin]) cur = '#';
        fprintf(stderr, "%c%c", cur, j+1<b.S_columns? ' ':'\n');
      }
  }

  void M_inspect(board const& b, mino const& m) const {
    if (M_hold != 0) fprintf(stderr, "holding %c-mino\n", M_hold);
    fprintf(stderr, "%c-mino: (%zu, %zu)\n", m.M_type, m.M_i, m.M_j);
    for (size_t i = 0; i < b.S_rows; ++i)
      for (size_t j = 0; j < b.S_columns; ++j) {
        char cur = '.';
        if (m.occupies(i, j)) cur = '=';
        if (b.M_board[i+b.S_margin][j+b.S_margin]) cur = '#';
        fprintf(stderr, "%c%c", cur, j+1<b.S_columns? ' ':'\n');
      }
  }
  
  void M_reset_minos() {
    M_minos = S_minos;
    std::shuffle(M_minos.begin(), M_minos.end(), M_rng);
  }

  char S_get_op() const {
    char res;
    do {
      res = getchar();
      fprintf(stderr, "op: %c\n", res);
    } while (!(res == EOF || !isspace(res)));
    return res;
  }

  bool M_set_mino(mino const& m, board& b) {
    {
      /* check */
      bool valid = false;
      for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j) {
          if (!m.M_block[i][j]) continue;
          size_t bi = m.M_i + i;
          size_t bj = m.M_j + j;
          if (b.out_of_range(bi+1, bj) || b.occupied(bi+1, bj)) {
            valid = true;
            break;
          }
        }
      if (!valid) return false;
    }

    {
      /* set */
      for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j) {
          if (!m.M_block[i][j]) continue;
          size_t bi = m.M_i + i;
          size_t bj = m.M_j + j;
          b.set(bi, bj, m.M_type);
        }
    }

    b.erase();
    return true;
  }

  void M_put_mino() {
    fprintf(stderr, "putting: %c\n", M_minos.back());
    mino m(M_minos.back());
    M_minos.pop_back();

    M_inspect(b, m);

    while (true) {
      char op = S_get_op();
      bool valid = true;
      switch (op) {
      case 'j':
        valid = m.move(+1, 0, b);
        break;
      case 'k':
        valid = m.move(-1, 0, b);
        break;
      case 'h':
        valid = m.move(0, -1, b);
        break;
      case 'l':
        valid = m.move(0, +1, b);
        break;

      case 'J':
        while (m.move(+1, 0, b)) {}
        M_set_mino(m, b);
        M_inspect(b);
        return;

      case 'f':
        valid = m.rotate(true, b);
        break;
      case 'd':
        valid = m.rotate(false, b);
        break;

      case 'z':
        // hold
        if (M_hold != 0) M_minos.push_back(M_hold);
        M_hold = m.M_type;
        return;
      }

      fprintf(stderr, valid? "valid\n":"invalid\n");
      M_inspect(b, m);
    }
  }

public:
  game() {
    M_reset_minos();
  }

  void play() {
    do {
      if (M_minos.empty()) M_reset_minos();
      M_put_mino();
    } while (true);
  }
};

int main() {
  game g;
  g.play();
}
