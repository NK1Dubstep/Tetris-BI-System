/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <algorithm>

#include "./tetris/tetris.hpp"
#include "render.hpp"
#define FONT8x16_IMPLEMENTATION
#include "font8x16.h"

namespace tetris_bi {
  std::vector<uint32_t>& render::get_buffer() {
    return buffer;
  }

  void render::draw_rectangle(const uint32_t x1, const uint32_t y1,
    const uint32_t x2, const uint32_t y2, const uint32_t color) {
    uint32_t x1_clamped = std::min(x1, width - 1);
    uint32_t y1_clamped = std::min(y1, height - 1);
    uint32_t x2_clamped = std::min(x2, width - 1);
    uint32_t y2_clamped = std::min(y2, height - 1);

    for (uint32_t y = y1_clamped; y <= y2_clamped; y++) {
      for (uint32_t x = x1_clamped; x <= x2_clamped; x++) {
        buffer[y * width + x] = color;
      }
    }
  }

  void render::draw_square(const uint32_t x1, const uint32_t y1,
    const uint32_t size, const uint32_t color) {
    draw_rectangle(x1, y1, x1 + size, y1 + size, color);
  }

  void render::draw_char(const char c, const uint32_t x1, const uint32_t y1, uint32_t color) {
    const uint32_t x1_upto = std::min(x1 + FONT_W, width);
    const uint32_t y1_upto = std::min(y1 + FONT_H, height);

    auto font = font8x16;

    const size_t idx = static_cast<uint8_t>(c);

    for (int y = y1; y < y1_upto; y++) {
      auto dy = y - y1;
      uint32_t row = 0;
      int bytes_per_row = (FONT_W + 7) / 8;
      for (int b = 0; b < bytes_per_row; b++) {
        row = (row << 8) | font[idx][dy * bytes_per_row + b];
      }
      uint8_t str = font[idx][dy];
      for (int x = x1; x < x1_upto; x++) {
        auto dx = x - x1;
        if (str & (1 << (FONT_W - 1 - dx))) {
          buffer[y * width + x] = color;
        }
      }
    }
  }

  void render::draw_string(const std::string &s, const uint32_t x1, const uint32_t y1, const uint32_t color) {
    int dx = 0, dy = 0;
    for (auto c : s) {
      if (c == '\n') {
        dx = 0;
        dy += FONT_H;
        continue;
      }
      draw_char(c, x1 + dx, y1 + dy, color);
      dx += FONT_W;
    }
  }

  void render::resize(const uint32_t W, const uint32_t H) {
    std::fill(buffer.begin(), buffer.end(), 0);
    width = W;
    height = H;
    buffer.resize(W * H, 0);
  }

  [[nodiscard]] uint32_t render::get_color(int val) const {
    switch (val) {
    case 0:
      return 0;
    case 1:
      return 0xfa0000; // red
    case 2:
      return 0xfae100; // yellow
    case 3:
      return 0x45ad03; // green
    case 4:
      return 0x0303ad; // blue
    case 5:
      return 0x9603ad; // purple
    case 6:
      return 0x03a2ad; // cyan
    default:
      return 0xe864a6;
    }
  }

  void render::render_tetris(const tetris_field& field) {
    std::fill(buffer.data(), buffer.data() + width * height, 0);

    uint32_t cell_size = 20;

    uint32_t field_w = field.get_width();
    uint32_t field_h = field.get_height();

    uint32_t offsetX = (width - field_w * cell_size) / 2;
    uint32_t offsetY = (height - field_h * cell_size) / 2;

    for (uint32_t y = 0; y < field_h; y++) {
      for (uint32_t x = 0; x < field_w; x++) {
        uint32_t color = get_color(field.get_cell(x, y));
        uint32_t cx = offsetX + x * cell_size;
        uint32_t cy = offsetY + y * cell_size;
        draw_square(cx, cy, cell_size, color);
      }
    }

    uint32_t grid_color = 0x222222;

    for (uint32_t x = 0; x < field_w + 1; x++) {
      uint32_t x1 = x, x2 = x1, y1 = 0, y2 = field_h;
      x1 *= cell_size, x2 *= cell_size, y1 *= cell_size, y2 *= cell_size;

      x1 += offsetX, x2 += offsetX, y1 += offsetY, y2 += offsetY;

      draw_rectangle(x1, y1, x2, y2, grid_color);
    }

    for (uint32_t y = 0; y < field_h + 1; y++) {
      uint32_t x1 = 0, x2 = field_w, y1 = y, y2 = y1;
      x1 *= cell_size, x2 *= cell_size, y1 *= cell_size, y2 *= cell_size;

      x1 += offsetX, x2 += offsetX, y1 += offsetY, y2 += offsetY;

      draw_rectangle(x1, y1, x2, y2, grid_color);
    }
  }
}
