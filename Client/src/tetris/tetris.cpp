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
  }

  void tetris_game::start_session() noexcept {
    if (st == state::SESSION) {
      end_session();
    }
    st = state::SESSION;
    prog = {};
    diff = {};
    tetris_field.reset();
    from_last_tick = 0;
    generate_new_figure(current_figure);
    commit_current_figure();
    generate_new_figure(next_figure);
  }

  void tetris_game::end_session() noexcept {
    if (st == state::SESSION) {
      played_sessions_stats.push_back({prog, diff});
      st = state::IDLE;
    }
  }

  [[nodiscard]] bool tetris_game::check_current_shape(const int x, const int y) const {
    for (auto& p : current_figure.points) {
      if (p.x == x && p.y == y) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] bool tetris_game::check_down() const {
    for (auto& p : current_figure.points) {
      bool is_yourself = check_current_shape(p.x, p.y + 1);
      if (tetris_field.get_cell(p.x, p.y + 1) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::move_down() {
    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, 0);
      p.y++;
    }
    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_direction(int dir) const {
    for (auto& p : current_figure.points) {
      bool is_yourself = check_current_shape(p.x + dir, p.y);
      if (tetris_field.get_cell(p.x + dir, p.y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::move_direction(int dir) {
    if (st != state::SESSION || !check_direction(dir)) return;

    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, 0);
      p.x += dir;
    }
    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_rotateCW() const {
    point rotate_point = current_figure.points[1];

    for (auto& p : current_figure.points) {
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_y = -new_y;
      new_x += rotate_point.x;
      new_y += rotate_point.y;
      bool is_yourself = check_current_shape(new_x, new_y);
      if (tetris_field.get_cell(new_x, new_y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::rotateCW() {
    if (current_figure.type == shape_type::O) return;
    if (st != state::SESSION || !check_rotateCW()) return;

    point rotate_point = current_figure.points[1];

    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, 0);
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_y = -new_y;
      p.x = new_x + rotate_point.x;
      p.y = new_y + rotate_point.y;
    }
    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  [[nodiscard]] bool tetris_game::check_rotateCCW() const {
    point rotate_point = current_figure.points[1];

    for (auto& p : current_figure.points) {
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_x = -new_x;
      new_x += rotate_point.x;
      new_y += rotate_point.y;
      bool is_yourself = check_current_shape(new_x, new_y);
      if (tetris_field.get_cell(new_x, new_y) != 0 && !is_yourself) {
        return false;
      }
    }
    return true;
  }

  void tetris_game::rotateCCW() {
    if (current_figure.type == shape_type::O) return;
    if (st != state::SESSION || !check_rotateCCW()) return;

    point rotate_point = current_figure.points[1];

    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, 0);
      int new_x = p.x - rotate_point.x;
      int new_y = p.y - rotate_point.y;
      std::swap(new_x, new_y);
      new_x = -new_x;
      p.x = new_x + rotate_point.x;
      p.y = new_y + rotate_point.y;
    }
    for (auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  void tetris_game::commit_current_figure() {
    for (const auto& p : current_figure.points) {
      tetris_field.change_cell(p.x, p.y, p.val);
    }
  }

  void tetris_game::generate_new_figure(tetris_figure& figure) {
    int figure_id = generate_randui32() % 7;
    figure.points = shapes_presets[figure_id];
    figure.type = shapes_types_presets[figure_id];
    int random_val = generate_randui32() % 8 + 1;
    for (auto& p : figure.points) {
      p.val = random_val;
    }
  }

  bool tetris_game::check_for_win() const {
    if (prog.figure_passed_lvl > diff.figure_to_win) {
      return true;
    }
    return false;
  }

  bool tetris_game::check_for_lose() const {
    for (const auto& p : current_figure.points) {
      if (p.y < diff.lose_line) {
        return true;
      }
    }
    return false;
  }

  void tetris_game::go_next_level() {
    diff.level_number++;
    diff.figure_to_win++;
    prog.figure_passed_lvl = 0;

    if (diff.level_number % 2 == 0) {
      diff.lose_line = std::min(diff.lose_line + 1, tetris_field.get_height() - 5);
    }
    tetris_field.reset();
  }

  void tetris_game::tick() {
    if (st != state::SESSION) return;

    if (check_down()) {
      move_down();
    }
    else {
      int cleared = tetris_field.clear_lines();
      prog.score += cleared * cleared;
      prog.total_lines += cleared;
      prog.figure_passed++, prog.figure_passed_lvl++;
      // std::cout << "succeessfuly clear " << cleared << " lines!!!\n";

      if (check_for_win()) {
        // std::cout << "You win!!!\n";
        go_next_level();
        generate_new_figure(current_figure);
        commit_current_figure();
        generate_new_figure(next_figure);
        return;
      }

      if (check_for_lose()) {
        // std::cout << "you lose:(\n";
        end_session();
        return;
      }

      current_figure = std::move(next_figure);
      commit_current_figure();
      generate_new_figure(next_figure);
      return;
    }
  }

  void tetris_game::update(const timer::seconds delta_time) {
    static constexpr timer::seconds FIGURE_FALL_INTERVAL = 0.05;

    if (st == state::SESSION) {
      from_last_tick += delta_time;
      if (from_last_tick > FIGURE_FALL_INTERVAL) {
        tick();
        from_last_tick = 0;
      }
    } else if (st == state::IDLE) {
      ;
    }
  }

  [[nodiscard]] tetris_game::state tetris_game::get_state() noexcept {
    return st;
  }

  std::optional<tetris_game::session_stats>
    tetris_game::pop_last_played_session_stats() {

    if (played_sessions_stats.empty()) {
      return std::nullopt;
    }
    auto stats = played_sessions_stats.back();
    played_sessions_stats.pop_back();
    return stats;
  }
}
