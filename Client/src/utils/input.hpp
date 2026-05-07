/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include <array>
#include <cstdint>
#include "MiniFB_cpp.h"

namespace tetris_bi {
  class input {
    static constexpr uint32_t MINIFB_MAX_KEY{349};
    static constexpr uint32_t MINIFB_MAX_MBUTTON{8};

    std::array<bool, MINIFB_MAX_KEY> pressed_keys{};
    std::array<bool, MINIFB_MAX_KEY> pressed_keys_instant{};

    int x{0}, y{0}, dx{0}, dy{0};

  public:
    void instant_reset() {
      std::fill(
        pressed_keys_instant.begin(),
        pressed_keys_instant.end(),
        false
      );
    }

    void update_keyboard(mfb_key key, bool is_pressed) {
      if (key == KB_KEY_UNKNOWN) {
        return;
      }

      if (is_pressed) {
        pressed_keys[key] = true;
        pressed_keys_instant[key] = true;
      } else {
        pressed_keys[key] = false;
      }
    }

    void update_mbuttons(mfb_mouse_button button, bool is_pressed) {
      if (is_pressed) {
        pressed_keys[button] = true;
        pressed_keys_instant[button] = true;
      } else {
        pressed_keys[button] = false;
      }
    }

    void update_mpos(int mx, int my, float dpi_x = 1.0, float dpi_y = 1.0) noexcept {
      dx = x - mx, dy = y - my;
      x = static_cast<int>(mx / dpi_x), y = static_cast<int>(my / dpi_y);
    }

    const auto &get_pressed_keys() const noexcept {
      return pressed_keys;
    }

    const auto &get_pressed_keys_instant() const noexcept {
      return pressed_keys_instant;
    }

    int get_x() const noexcept { return x; }
    int get_y() const noexcept { return y; }
    int get_dx() const noexcept { return dx; }
    int get_dy() const noexcept { return dy; }
  };
}