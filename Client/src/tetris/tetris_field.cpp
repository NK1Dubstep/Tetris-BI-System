/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "tetris.hpp"

namespace tetris_bi {
  [[nodiscard]] int tetris_field::get_cell(int x, int y) const {
    if (x < 0 || x >= m_field_width || y < 0 || y >= m_field_height) {
      return -1;
    }
    return tetris_field_[x][y];
  }

  void tetris_field::change_cell(const int x, const int y, const int val) {
    if (x < 0 || x >= m_field_width || y < 0 || y >= m_field_height) {
      return;
    }
    tetris_field_[x][y] = val;
  }

  bool tetris_field::is_line_full(const int row) {
    if (row < 0 || row >= m_field_height) {
      return false;
    }

    for (int x = 0; x < m_field_width; x++) {
      if (tetris_field_[x][row] == 0) {
        return false;
      }
    }
    return true;
  }

  void tetris_field::remove_line(const int row) {
    if (row < 0 || row >= m_field_height) return;

    for (int y = row; y > 0; y--) {
      for (int x = 0; x < m_field_width; x++) {
        int val = tetris_field_[x][y - 1];
        tetris_field_[x][y] = val;
      }
    }

    for (int x = 0; x < m_field_width; x++) {
      tetris_field_[x][0] = 0;
    }
  }

  int tetris_field::clear_lines() {
    int cleared = 0;

    for (int y = m_field_height - 1; y >= 0; ) {
      if (is_line_full(y)) {
        remove_line(y);
        cleared++;
      }
      else {
        y--;
      }
    }
    return cleared;
  }

  void tetris_field::reset() {
    for (int x = 0; x < m_field_width; x++) {
      for (int y = 0; y < m_field_height; y++) {
        tetris_field_[x][y] = 0;
      }
    }
  }
}
