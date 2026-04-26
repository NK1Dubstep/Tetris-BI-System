/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <mutex>

#include "window/window.hpp"
#include "render/render.hpp"
#include "tetris/tetris.hpp"
#include "tetris/bot/bots_client.hpp"

namespace tetris_bi {
  class super_tetris_window : public window, public render {
  public:
    super_tetris_window();

  private:
    void my_active(mfb_window*, bool is_active) override;
    void my_resize(mfb_window*, int w, int h) override;
    bool my_close(mfb_window*) override;
    void my_keyboard(mfb_window*, mfb_key key, mfb_key_mod mod, bool is_pressed) override;
    void my_char_input(mfb_window*, uint32_t char_code) override;
    void my_mouse_button(mfb_window*, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed) override;
    void my_mouse_move(mfb_window*, int x, int y) override;
    void my_mouse_scroll(mfb_window*, mfb_key_mod mod, float dx, float dy) override;

    void my_frame() override;

    const int BOT_NUMBER = 10000;

    bots_client bsc = bots_client(BOT_NUMBER);
    timer tim;
    int draw_index{0};
  };
}
