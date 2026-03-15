/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <algorithm>

#include "render.hpp"

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

void render::render_tetris(const std::vector<std::vector<uint32_t>>& tetris_field) {
  std::fill(m_buffer.data(), m_buffer.data() + m_W * m_H, 0);

  
  uint32_t cell_size = 10;

  uint32_t field_w = tetris_field.size();
  uint32_t field_h = tetris_field[0].size();

  uint32_t offsetX = (m_W - field_w * cell_size) / 2;
  uint32_t offsetY = (m_H - field_h * cell_size) / 2;

  for (uint32_t y = 0; y < field_h; y++) {
    for (uint32_t x = 0; x < field_w; x++) {
      uint32_t color = 250 * tetris_field[x][y];
      uint32_t cx = offsetX + x * cell_size;
      uint32_t cy = offsetY + y * cell_size;
      draw_square(cx, cy, cell_size, color);
    }
  }
}
