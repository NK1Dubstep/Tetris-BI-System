/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "tetris.hpp"

namespace tetris_bi {
  void tetris_game::generate_shapes_data() {
    shapes_presets =
    {
      {
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     1, 0}
      }, // O-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 + 1, 1, 0}
      }, // I-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2,     0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0}
      }, // S-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     1, 0}
      }, // Z-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2,     0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     1, 0}
      }, // L-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 1, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     1, 0}
      }, // J-figure
      {
        {static_cast<int>(tetris_field.get_width()) / 2 - 2, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2,     0, 0},
        {static_cast<int>(tetris_field.get_width()) / 2 - 1, 1, 0}
      } // T-figure
    };
    shapes_types_presets =
    {
      shape_type::O,
      shape_type::I,
      shape_type::S,
      shape_type::Z,
      shape_type::L,
      shape_type::J,
      shape_type::T
    };
  }
}
