/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include <vector>
#include "tetris/tetris.hpp"

namespace tetris_bi {
  class render {
  public:
    render(const uint32_t W, const uint32_t H) : width(W), height(H), buffer(W * H, 0) {}

    std::vector<uint32_t>& get_buffer();

    void draw_rectangle(const uint32_t x1, const uint32_t y1,
      const uint32_t x2, const uint32_t y2, const uint32_t color);

    void draw_square(const uint32_t x1, const uint32_t y1,
      const uint32_t size, const uint32_t color);

    void resize(const uint32_t W, const uint32_t H);

    [[nodiscard]] uint32_t get_color(int value) const;
    void render_tetris(const tetris_field& field);

    void draw_char(char c, uint32_t x1, uint32_t y1, uint32_t color);
    void draw_string(const std::string &s, uint32_t x1, uint32_t y1, uint32_t color);

  private:
    static const uint32_t FONT_W = 8, FONT_H = 16;
    uint32_t width;
    uint32_t height;
    std::vector<uint32_t> buffer;
  };
}
