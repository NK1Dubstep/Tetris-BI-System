/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "super_tetris_window.hpp"

#include "./tetris/tetris.hpp"

#include <iostream>
#include <format>

#include <chrono>
#include <thread>

namespace tetris_bi {

  super_tetris_window::super_tetris_window() :
    window("Nk1 av1 super tertis", 800, 800),
    render(800, 800)
  {
    mfb_update_ex(win, render::get_buffer().data(), width, height);
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
    mfb_update_ex(win, render::get_buffer().data(), width, height);
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

    if (is_pressed && m_tetris_games) {
      switch (key) {
      case mfb_key::KB_KEY_D:
        m_draw_index = (m_draw_index + 1) % m_tetris_games->size();
        break;
      case mfb_key::KB_KEY_A:
        m_draw_index = (m_draw_index - 1 + m_tetris_games->size()) % m_tetris_games->size();
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
    {
      if (!m_games_mutex || !m_tetris_games) return;

      std::lock_guard lg(*m_games_mutex);
      render_tetris((*m_tetris_games)[m_draw_index].get_tetris_field());
    }
    mfb_update_ex(win, render::get_buffer().data(), width, height);
  }
}