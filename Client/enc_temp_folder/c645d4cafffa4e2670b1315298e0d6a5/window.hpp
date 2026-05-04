/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include "MiniFB_cpp.h"

namespace tetris_bi {
  class window {
  protected:
    mfb_window *win{nullptr};
    uint32_t width{}, height{};

    virtual void my_active(mfb_window *, bool is_active);
    virtual void my_resize(mfb_window *, int w, int h);
    virtual bool my_close(mfb_window *);
    virtual void my_keyboard(mfb_window *, mfb_key key, mfb_key_mod mod, bool is_pressed);
    virtual void my_char_input(mfb_window *, uint32_t char_code);
    virtual void my_mouse_button(mfb_window *, mfb_mouse_button button, mfb_key_mod mod, bool is_pressed);
    virtual void my_mouse_move(mfb_window *, int x, int y);
    virtual void my_mouse_scroll(mfb_window *, mfb_key_mod mod, float dx, float dy);

    virtual void my_frame();

  public:
    window(const char *title, uint32_t width, uint32_t height, uint32_t flags = WF_RESIZABLE);

    void run();
  };
}
