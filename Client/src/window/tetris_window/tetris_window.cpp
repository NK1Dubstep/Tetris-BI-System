/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "tetris_window.hpp"

#include "./tetris/tetris.hpp"

#include <iostream>
#include <format>

#include <chrono>
#include <thread>

namespace tetris_bi {

  tetris_window::tetris_window() :
    window("Nk1 av1 super tertis", 800, 800),
    render(800, 800)
  {
    // render::draw_rectangle(0, 0, 100, 100, 0x834d18);
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }

  void tetris_window::my_active(
    mfb_window *,
    [[maybe_unused]] bool is_active
  ) {
    // std::cout << std::format("(MiniFB callback called, my_active) is_active: {}\n", is_active);
  }

  void tetris_window::my_resize(
    mfb_window *,
    [[maybe_unused]] int w,
    [[maybe_unused]] int h
  ) {
    window::my_resize(nullptr, w, h);

    render::resize(w, h);
    render::draw_rectangle(0, 0, 100, 100, 0x834d18);
    render::draw_string("ABOBA", 0, 0, 0);
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
    // std::cout << std::format("(MiniFB callback called, my_resize) Was resized to: {} {}\n", w, h);
  }

  bool tetris_window::my_close(
    mfb_window *
  ) {
    // std::cout << "(MiniFB callback called, my_close) Close trigger\n";
    return true;
  }

  void tetris_window::my_keyboard(
    mfb_window *,
    [[maybe_unused]] mfb_key key,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
    // std::cout << std::format("(MiniFB callback called, my_keyboard), Press flag: {}\n", is_pressed);

    if (is_pressed) {
      if (mod & mfb_key_mod::KB_MOD_CONTROL && key == mfb_key::KB_KEY_S) {
        m_tetris->start_session();
        return;
      }

      if (mod & mfb_key_mod::KB_MOD_CONTROL && key == mfb_key::KB_KEY_E) {
        m_tetris->end_session();
        return;
      }

      switch (key) {
        case mfb_key::KB_KEY_A:
          m_tetris->move_direction(-1);
          break;
        case mfb_key::KB_KEY_D:
          m_tetris->move_direction(1);
          break;
        case mfb_key::KB_KEY_S:
          m_tetris->rotateCW();
          break;
        case mfb_key::KB_KEY_W:
          m_tetris->rotateCCW();
          break;
        case mfb_key::KB_KEY_P:
          tim.pause_switch();
          break;
        default:
          break;
      }
    }
  }

  void tetris_window::my_char_input(
    mfb_window *,
    [[maybe_unused]] uint32_t char_code
  ) {
    // std::cout << std::format("(MiniFB callback called, my_char_input) char: {}\n", static_cast<char>(char_code));
  }

  void tetris_window::my_mouse_button(
    mfb_window *,
    [[maybe_unused]] mfb_mouse_button button,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
    // std::cout << std::format("(MiniFB callback called, my_mouse_button)\n");
  }

  void tetris_window::my_mouse_move(
    mfb_window *,
    [[maybe_unused]] int x,
    [[maybe_unused]] int y
  ) {
    // std::cout << std::format("(MiniFB callback called, my_mouse_move) Mouse moved to: {} {}\n", x, y);
  }

  void tetris_window::my_mouse_scroll(
    mfb_window *,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] float dx,
    [[maybe_unused]] float dy
  ) {
    // std::cout << std::format("(MiniFB callback called, my_mouse_scroll) dx: {}, dy: {}\n", dx, dy);
  }

  void tetris_window::my_frame() {
    tim.update();
    m_tetris->update(tim.delta_time_p);
    render_tetris(m_tetris->get_tetris_field());

    auto prog = m_tetris->get_prog(); auto diff = m_tetris->get_diff();

    auto str1 = std::format(R"(Nk1 av1 super tertis, FPS: {})", tim.fps);
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
    draw_string(str4, 0, 48, 0x666666);
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }
}
