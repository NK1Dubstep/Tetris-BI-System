/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <cstdlib>
#include <cstdint>
#include <iostream>

#include "utils/random.hpp"
#include "tetris.hpp"

namespace tetris_bi {
  tetris_game::tetris_game() {
    generate_shapes_data();
    generate_new_figure(m_current_figure);
    commit_current_figure();
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
    if (is_game_over || !check_direction(dir)) return;

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
    if (is_game_over || !check_rotateCW()) return;

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
    if (is_game_over || !check_rotateCCW()) return;

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

  void tetris_game::commit_current_figure() {
    for (const auto& p : m_current_figure.points) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  void tetris_game::generate_new_figure(tetris_figure& figure) {
    int figure_id = generate_randui32() % 7;
    figure.points = m_shapes_presets[figure_id];
    figure.type = m_shapes_types_presets[figure_id];
    int random_val = generate_randui32() % 8 + 1;
    for (auto& p : figure.points) {
      p.val = random_val;
    }
  }

  bool tetris_game::check_for_win() const {
    if (m_figure_passed_lvl > m_figure_to_win) {
      return true;
    }
    return false;
  }

  bool tetris_game::check_for_lose() const {
    for (const auto& p : m_current_figure.points) {
      if (p.y < m_lose_line) {
        return true;
      }
    }
    return false;
  }

  void tetris_game::go_next_level() {
    m_level_number++;
    m_figure_to_win++;
    m_figure_passed_lvl = 0;
    if (m_level_number % 2 == 0) {
      m_lose_line = std::min(m_lose_line + 1, m_tetris_field.get_height() - 5);
    }
    m_tetris_field.reset();
  }

  void tetris_game::tick() {
    if (is_game_over) return;

    if (check_down()) {
      move_down();
    }
    else {
      int cleared = m_tetris_field.clear_lines();
      m_score += cleared * cleared;
      m_total_lines += cleared;
      m_figure_passed++, m_figure_passed_lvl++;
      std::cout << "succeessfuly clear " << cleared << " lines!!!\n";

      if (check_for_win()) {
        std::cout << "You win!!!\n";
        go_next_level();
        generate_new_figure(m_current_figure);
        commit_current_figure();
        generate_new_figure(m_next_figure);
        return;
      }
      
      if (check_for_lose()) {
        std::cout << "you lose:(\n";
        is_game_over = true;
        return;
      }
      
      m_current_figure = std::move(m_next_figure);
      commit_current_figure();
      generate_new_figure(m_next_figure);
      return;
    }
  }

  void tetris_game::update(const timer::seconds delta_time) {
    static constexpr timer::seconds FIGURE_FALL_INTERVAL = 0.2;

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
