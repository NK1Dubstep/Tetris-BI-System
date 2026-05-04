/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include "window/window.hpp"
#include "render/render.hpp"
#include "tetris/tetris.hpp"

namespace tetris_bi {
  class tetris_window : public window, public render {
  public:
    tetris_window();

    void link_tetris(tetris_game& game) {
      m_tetris = &game;
    }

  private:
    void my_active(mfb_window *, bool is_active) override;
    void my_resize(mfb_window *, int w, int h) override;
    bool my_close(mfb_window *) override;
    void my_keyboard(mfb_window *, mfb_key key, mfb_key_mod mod, bool is_pressed) override;
    void my_char_input(mfb_window *, uint32_t char_code) override;
    void my_mouse_button(mfb_window *, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed) override;
    void my_mouse_move(mfb_window *, int x, int y) override;
    void my_mouse_scroll(mfb_window *, mfb_key_mod mod, float dx, float dy) override;

    void my_frame() override;

    timer tim;  // mega timer
    tetris_game* m_tetris = nullptr;
  };
}
