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
    static constexpr uint32_t MINIFB_MAX_KEY{512};
    static constexpr uint32_t MINIFB_MAX_MBUTTON{8};

    std::array<bool, MINIFB_MAX_KEY> pressed_keys{};
    std::array<bool, MINIFB_MAX_KEY> pressed_keys_prev{};
    std::array<bool, MINIFB_MAX_KEY> pressed_keys_instant{};

    int x{0}, y{0}, dx{0}, dy{0};

  public:
    void update_keyboard(mfb_window *win) {
      const uint8_t *minifb_pressed_keys = mfb_get_key_buffer(win);

      for (uint32_t key = MINIFB_MAX_MBUTTON; key < MINIFB_MAX_KEY; key++) {
        pressed_keys[key] = minifb_pressed_keys[key];

        if (!pressed_keys_prev[key]) {
          pressed_keys_instant[key] = minifb_pressed_keys[key];
        } else {
          pressed_keys_instant[key] = false;
        }
      }
      std::copy(
        pressed_keys.begin() + MINIFB_MAX_MBUTTON,
        pressed_keys.end(),
        pressed_keys_prev.begin() + MINIFB_MAX_MBUTTON
      );
    }

    void update_mbuttons(mfb_window *win) {
      const uint8_t *minifb_pressed_mbuttons = mfb_get_mouse_button_buffer(win);

      for (uint32_t key = 0; key < MINIFB_MAX_MBUTTON; key++) {
        pressed_keys[key] = minifb_pressed_mbuttons[key];

        if (!pressed_keys_prev[key]) {
          pressed_keys_instant[key] = minifb_pressed_mbuttons[key];
        } else {
          pressed_keys_instant[key] = false;
        }
      }
      std::copy(
        pressed_keys.begin(),
        pressed_keys.begin() + MINIFB_MAX_MBUTTON,
        pressed_keys_prev.begin()
      );
    }

    void update_mpos(mfb_window *win, float dpi_x = 1.0, float dpi_y = 1.0) noexcept {
      int mx = mfb_get_mouse_x(win), my = mfb_get_mouse_y(win);
      mx = static_cast<int>(mx / dpi_x), my = static_cast<int>(my / dpi_y);

      dx = mx - x, dy = my - y;
      x = mx, y = my;
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