/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <vector>

class render {
public:
  render(const uint32_t W, const uint32_t H) : m_W(W), m_H(H), m_buffer(W * H, 0) {}

  std::vector<uint32_t>& get_buffer();

  void draw_rectangle(const uint32_t x1, const uint32_t y1,
    const uint32_t x2, const uint32_t y2, const uint32_t color);

  void draw_square(const uint32_t x1, const uint32_t y1,
    const uint32_t size, const uint32_t color);

  void resize(const uint32_t W, const uint32_t H);

  void render_tetris(const std::vector<std::vector<uint32_t>>& tetris_field);
private:
  uint32_t m_W;
  uint32_t m_H;
  std::vector<uint32_t> m_buffer;
};
