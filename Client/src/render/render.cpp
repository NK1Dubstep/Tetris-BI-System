/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <algorithm>

#include "./tetris/tetris.hpp"
#include "render.hpp"

namespace tetris_bi {
  std::vector<uint32_t>& render::get_buffer() {
    return m_buffer;
  }

  void render::draw_rectangle(const uint32_t x1, const uint32_t y1,
    const uint32_t x2, const uint32_t y2, const uint32_t color) {
    uint32_t x1_clamped = std::min(x1, m_W - 1);
    uint32_t y1_clamped = std::min(y1, m_H - 1);
    uint32_t x2_clamped = std::min(x2, m_W - 1);
    uint32_t y2_clamped = std::min(y2, m_H - 1);

    for (uint32_t y = y1_clamped; y <= y2_clamped; y++) {
      for (uint32_t x = x1_clamped; x <= x2_clamped; x++) {
        m_buffer[y * m_W + x] = color;
      }
    }
  }

  void render::draw_square(const uint32_t x1, const uint32_t y1,
    const uint32_t size, const uint32_t color) {
    draw_rectangle(x1, y1, x1 + size, y1 + size, color);
  }

  void render::resize(const uint32_t W, const uint32_t H) {
    std::fill(m_buffer.data(), m_buffer.data() + m_W * m_H, 0);
    m_W = W;
    m_H = H;
    m_buffer.resize(W * H, 0);
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
    std::fill(m_buffer.data(), m_buffer.data() + m_W * m_H, 0);

    uint32_t cell_size = 20;

    uint32_t field_w = field.get_width();
    uint32_t field_h = field.get_height();

    uint32_t offsetX = (m_W - field_w * cell_size) / 2;
    uint32_t offsetY = (m_H - field_h * cell_size) / 2;

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
