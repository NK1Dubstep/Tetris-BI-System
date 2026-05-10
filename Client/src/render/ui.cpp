/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include <algorithm>
#include <sstream>

#include "utils/audio.hpp"
#include "ui.hpp"

#undef max

namespace tetris_bi {

  button::button(int x1, int y1, std::string text,
    uint32_t trigger_key, const std::function<void()> &f) :
    x1(x1), y1(y1), text(text),
    y2(y1 + MARGIN_Y + (static_cast<uint32_t>(std::count(text.begin(),
      text.end(), '\n')) + 1) * render::FONT_H),
    trigger_key(trigger_key),
    f(f) {

    std::istringstream stream(text);
    std::string line;
    size_t max_len = 0;

    while (std::getline(stream, line)) {
      max_len = std::max(max_len, line.size());
    }
    x2 = x1 + MARGIN_X + static_cast<int>(max_len) * render::FONT_W;
  }

  void button::draw(render &rnd) {
    const uint32_t inner_color = is_hovered ? (is_hold ? 0xAAAAAA : 0x777777) : 0x444444;
    const uint32_t border_color =  inner_color + 0x111111;

    rnd.draw_rectangle(x1, y1, x2, y2, inner_color);
    rnd.draw_rectangle(x1, y1, x1 + 1, y2, border_color);
    rnd.draw_rectangle(x2 - 1, y1, x2, y2, border_color);
    rnd.draw_rectangle(x1, y1, x2, y1 + 1, border_color);
    rnd.draw_rectangle(x1, y2 - 1, x2, y2, border_color);
    rnd.draw_string(text, x1 + MARGIN_X / 2, y1 + MARGIN_Y / 2, 0xFFFFFF);
  }

  void button::update(const input_state &state) {
    is_hovered = is_hold = is_pressed = false;

    if (x1 <= state.x && state.x <= x2 && y1 <= state.y && state.y <= y2) {
      is_hovered = true;

      is_hold = state.pressed_keys[trigger_key];
      is_pressed = state.pressed_keys_instant[trigger_key];
    }

    if (is_pressed) {
      audio_player::get().play("music/Mecha3Perc7.wav");
      f();
    }
  }
}
