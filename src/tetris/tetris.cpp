/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <immintrin.h>

#include "tetris.hpp"

namespace tetris_bi {
  tetris_game::tetris_game() {
    generate_shapes_data();
    generate_new_figure(m_current_figure);
    for (auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
    generate_new_figure(m_next_figure);
  }

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
    auto f = []() {
      uint32_t rd;
      while (!_rdrand32_step(&rd)) ;
      return rd;
    };

    int figure_id = f() % 7;
    figure.points = m_shapes_presets[figure_id];
    figure.type = m_shapes_types_presets[figure_id];
    int random_val = f() % 8 + 1;
    for (auto& p : figure.points) {
      p.val = random_val;
    }
  }

  void tetris_game::tick() {
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

  void tetris_game::update(const timer::seconds delta_time) {
    static constexpr timer::seconds FIGURE_FALL_INTERVAL = 0.1;

    if (st == state::SESSION) {
      from_last_tick += delta_time;
      if (from_last_tick > FIGURE_FALL_INTERVAL) {
        tick();
        from_last_tick = 0;
      }
    } else if (st == state::IDLE) {
      // TODO: multiple sessions (idle -> session -> idle -> session)
      // when start game:::--> state = SESSION, from_last_tick = 0;
    }
  }
}
