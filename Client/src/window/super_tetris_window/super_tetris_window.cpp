/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "super_tetris_window.hpp"

#include "./tetris/tetris.hpp"

#include <iostream>
#include <format>

#include <chrono>
#include <thread>

namespace tetris_bi {

  const int super_tetris_window::DUMMY_BOT_NUMBER = 100;
  const int super_tetris_window::SMART_BOT_NUMBER = 3;
  const int super_tetris_window::BOT_NUMBER =
    super_tetris_window::DUMMY_BOT_NUMBER + super_tetris_window::SMART_BOT_NUMBER;
  super_tetris_window::super_tetris_window() :
    window("Nk1 av1 super tertis", 800, 800),
    render(800, 800)
  {
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }

  void super_tetris_window::my_active(
    mfb_window*,
    [[maybe_unused]] bool is_active
  ) {
  }

  void super_tetris_window::my_resize(
    mfb_window*,
    [[maybe_unused]] int w,
    [[maybe_unused]] int h
  ) {
    window::my_resize(nullptr, w, h);

    render::resize(w, h);
    render::draw_rectangle(0, 0, 100, 100, 0x834d18);
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }

  bool super_tetris_window::my_close(
    mfb_window*
  ) {
    return true;
  }

  void super_tetris_window::my_keyboard(
    mfb_window*,
    [[maybe_unused]] mfb_key key,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {

    if (is_pressed) {
      switch (key) {
      case mfb_key::KB_KEY_D:
        draw_index = (draw_index + 1) % BOT_NUMBER;
        break;
      case mfb_key::KB_KEY_A:
        draw_index = (draw_index - 1 + BOT_NUMBER) % BOT_NUMBER;
        break;
      default:
        break;
      }
    }
  }

  void super_tetris_window::my_char_input(
    mfb_window*,
    [[maybe_unused]] uint32_t char_code
  ) {
  }

  void super_tetris_window::my_mouse_button(
    mfb_window*,
    [[maybe_unused]] mfb_mouse_button button,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
  }

  void super_tetris_window::my_mouse_move(
    mfb_window*,
    [[maybe_unused]] int x,
    [[maybe_unused]] int y
  ) {
  }

  void super_tetris_window::my_mouse_scroll(
    mfb_window*,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] float dx,
    [[maybe_unused]] float dy
  ) {
  }

  void super_tetris_window::my_frame() {
    tim.update();
    bsc.update();

    std::lock_guard guard(bsc.bots_mutex);
    auto &target_tetris = bsc.bots[draw_index]->game;

    render_tetris(target_tetris.get_tetris_field());
    auto prog = target_tetris.get_prog(); auto diff = target_tetris.get_diff();

    auto str1 = std::format(R"(Nk1 av1 super tertis, FPS: {})", tim.fps);
    auto str2 = std::format(R"(Bot index: {})", draw_index);
    auto str3 = std::format(R"(Total bots: {})", BOT_NUMBER);
    auto str4 = std::format(
      R"(Tertis prog score: {}
Tertis prog total_lines: {},
Tertis prog figure_passed: {},
Tertis prog figure_passed_lvl: {},
Tertis diff level_number: {},
Tertis diff figure_to_win: {},
Tertis diff lose_line: {},
)", prog.score, prog.total_lines, prog.figure_passed, prog.figure_passed_lvl,
    diff.level_number, diff.figure_to_win, diff.lose_line);

    draw_string(str1, 0, 0, 0x834d18);
    draw_string(str3, 0, 16, 0x006600);
    draw_string(str2, 0, 32, 0x660066);
    draw_string(str4, 0, 48, 0x666666);

    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }
}
