/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "tetris.hpp"

namespace tetris_bi {
  [[nodiscard]] int tetris_field::get_cell(int x, int y) const {
    if (x < 0 || x >= m_field_width || y < 0 || y >= m_field_height) {
      return -1;
    }
    return m_tetris_field_[x][y];
  }

  void tetris_field::change_cell(const int x, const int y, const int val) {
    if (x < 0 || x >= m_field_width || y < 0 || y >= m_field_height) {
      return;
    }
    m_tetris_field_[x][y] = val;
  }

  bool tetris_field::is_line_full(const int row) {
    if (row < 0 || row >= m_field_height) {
      return false;
    }

    for (int x = 0; x < m_field_width; x++) {
      if (m_tetris_field_[x][row] == 0) {
        return false;
      }
    }
    return true;
  }

  void tetris_field::remove_line(const int row) {
    if (row < 0 || row >= m_field_height) return;

    for (int y = row; y > 0; y--) {
      for (int x = 0; x < m_field_width; x++) {
        int val = m_tetris_field_[x][y - 1];
        m_tetris_field_[x][y] = val;
      }
    }

    for (int x = 0; x < m_field_width; x++) {
      m_tetris_field_[x][0] = 0;
    }
  }

  int tetris_field::clear_lines() {
    int cleared = 0;

    for (int y = m_field_height - 1; y >= 0; ) {
      if (is_line_full(y)) {
        remove_line(y);
        cleared++;
      }
      else {
        y--;
      }
    }
    return cleared;
  }

  /* tetris_game */

  [[nodiscard]] bool tetris_game::check_current_shape(const int x, const int y) const {
    for (auto& p : m_current_figure.points) {
      if (p.x == x && p.y == y) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] bool tetris_game::check_down() const {
    for (auto& p : m_current_figure.points) {
      bool is_yourself = check_current_shape(p.x, p.y + 1);
      if (m_tetris_field.get_cell(p.x, p.y + 1) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::move_down() {
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, 0);
      p.y++;
    }
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_direction(int dir) const {
    for (auto& p : m_current_figure.points) {
      bool is_yourself = check_current_shape(p.x + dir, p.y);
      if (m_tetris_field.get_cell(p.x + dir, p.y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::move_direction(int dir) {
    if (!check_direction(dir)) return;

    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, 0);
      p.x += dir;
    }
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_rotateCW() const {
    point rotate_point = m_current_figure.points[1];

    for (auto& p : m_current_figure.points) {
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_y = -new_y;
      new_x += rotate_point.x;
      new_y += rotate_point.y;
      bool is_yourself = check_current_shape(new_x, new_y);
      if (m_tetris_field.get_cell(new_x, new_y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::rotateCW() {
    if (m_current_figure.type == shape_type::O) return;
    if (!check_rotateCW()) return;

    point rotate_point = m_current_figure.points[1];

    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, 0);
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_y = -new_y;
      p.x = new_x + rotate_point.x;
      p.y = new_y + rotate_point.y;
    }
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_rotateCCW() const {
    point rotate_point = m_current_figure.points[1];

    for (auto& p : m_current_figure.points) {
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_x = -new_x;
      new_x += rotate_point.x;
      new_y += rotate_point.y;
      bool is_yourself = check_current_shape(new_x, new_y);
      if (m_tetris_field.get_cell(new_x, new_y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::rotateCCW() {
    if (m_current_figure.type == shape_type::O) return;
    if (!check_rotateCCW()) return;

    point rotate_point = m_current_figure.points[1];

    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, 0);
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_x = -new_x;
      p.x = new_x + rotate_point.x;
      p.y = new_y + rotate_point.y;
    }
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  void tetris_game::generate_new_figure(tetris_figure& figure) {
    int figure_id = rand() % 7;
    figure.points = m_shapes_presets[figure_id];
    figure.type = m_shapes_types_presets[figure_id];
    int random_val = rand() % 8 + 1;
    for (auto& p : figure.points) {
      p.val = random_val;
    }
  }

  void tetris_game::update() {
    if (is_game_over) return;

    // int rand_dir = rand() % 2 * 2 - 1;
    // move_direction(rand_dir);
    // rotateCCW();

    if (check_down()) {
      move_down();
    }
    else {
      int cleared = m_tetris_field.clear_lines();
      m_score += cleared * cleared;
      m_total_lines += cleared;
      m_figure_passed++;
      std::cout << "succeessfuly clear " << cleared << " lines!!!\n";
      m_current_figure = std::move(m_next_figure);
      generate_new_figure(m_next_figure);
      return;
    }
  }
}