/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

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
    mfb_update_ex(win, render::get_buffer().data(), width, height);
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
    mfb_update_ex(win, render::get_buffer().data(), width, height);
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
    mfb_update_ex(win, render::get_buffer().data(), width, height);
  }

}
