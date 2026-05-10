/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#define MINIFB_IMPLEMENTATION
#include "window.hpp"

#include <stdexcept>

namespace tetris_bi {
  window::window(const char *title, uint32_t width, uint32_t height, uint32_t flags)
    : width(width), height(height), win(mfb_open_ex(title, width, height, flags))
  {
    if (win == nullptr) {
      throw std::runtime_error("Window creation FAIL");
    }
    // mfb_set_target_fps(1000);
    mfb_set_active_callback(win, this, &window::my_active);
    mfb_set_resize_callback(win, this, &window::my_resize);
    mfb_set_close_callback(win, this, &window::my_close);
    mfb_set_keyboard_callback(win, this, &window::my_keyboard);
    mfb_set_char_input_callback(win, this, &window::my_char_input);
    mfb_set_mouse_button_callback(win, this, &window::my_mouse_button);
    mfb_set_mouse_move_callback(win, this, &window::my_mouse_move);
    mfb_set_mouse_scroll_callback(win, this, &window::my_mouse_scroll);
    mfb_get_monitor_scale(win, &dpi_x, &dpi_y);
  }

  void window::my_active(
    mfb_window *,
    [[maybe_unused]] bool is_active
  ) {
  }

  void window::my_resize(
    mfb_window *,
    [[maybe_unused]] int w,
    [[maybe_unused]] int h
  ) {
    width = w;
    height = h;
  }

  bool window::my_close(
    mfb_window *
  ) {
    return true;
  }

  void window::my_keyboard(
    mfb_window *,
    [[maybe_unused]] mfb_key key,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
  }

  void window::my_char_input(
    mfb_window *,
    [[maybe_unused]] uint32_t char_code
  ) {
  }

  void window::my_mouse_button(
    mfb_window *,
    [[maybe_unused]] mfb_mouse_button button,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
  }

  void window::my_mouse_move(
    mfb_window *,
    [[maybe_unused]] int x,
    [[maybe_unused]] int y
  ) {
  }

  void window::my_mouse_scroll(
    mfb_window *,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] float dx,
    [[maybe_unused]] float dy
  ) {
  }

  void window::my_frame() {
  }

  void window::run() {
    do {
      my_frame();
    } while(mfb_wait_sync(win));
  }
}
