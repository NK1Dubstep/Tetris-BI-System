/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include "utils/timer.hpp"
#include <atomic>
#include <vector>

namespace tetris_bi {
  class tetris_field {
  public:
    [[nodiscard]] int get_cell(int x, int y) const;
    void change_cell(const int x, const int y, const int val);

    constexpr uint32_t get_width() const {
      return m_field_width;
    }

    constexpr uint32_t get_height() const {
      return m_field_height;
    }

    int clear_lines();

    void reset();

  private:

    void remove_line(const int row);
    bool is_line_full(const int row);

    uint32_t m_field_width{10};
    uint32_t m_field_height{20};
    std::vector<std::vector<int>> tetris_field_ =
      std::vector<std::vector<int>>(m_field_width, std::vector<int>(m_field_height));
  };

  enum class shape_type {
    O, I, S, Z, L, J, T
  };

  class tetris_game {
  public:
    tetris_game();

    const tetris_field &get_tetris_field() const {
      return tetris_field;
    }

    void move_direction(int dir);

    void rotateCW();
    void rotateCCW();

    void update(timer::seconds delta_time);

    void start_session() noexcept;
    void end_session(bool is_lose = false) noexcept;

    enum class state : short {
      IDLE, SESSION
    };

    [[nodiscard]] state get_state() noexcept;

  private:
    void generate_shapes_data();
    void tick();

    struct point {
      point(int x = 0, int y = 0, int val = 0) : x(x), y(y), val(val) {}

      int x = 0;
      int y = 0;
      int val = 0;
    };

    using tetris_shape = std::vector<point>;

    struct tetris_figure {
      tetris_figure() {}
      tetris_figure(tetris_shape points_, shape_type type_) : points(std::move(points_)), type(type_) {}

      tetris_shape points;
      shape_type type = shape_type::O;
    };

    [[nodiscard]] bool check_current_shape(const int x, const int y) const;
  public:
    tetris_figure get_current_figure() const {
      return current_figure;
    }

    int get_current_figure_high() const {
      int h = 1e9;
      for (auto& p : current_figure.points) {
        if (p.y < h) h = p.y;
      }
      return h;
    }
  private:

    [[nodiscard]] bool check_down() const;
    void move_down();
  public:
    void drop();
  private:

    [[nodiscard]] bool check_direction(int dir) const;

    [[nodiscard]] bool check_rotateCW() const;
    [[nodiscard]] bool check_rotateCCW() const;

    void commit_current_figure();
    void generate_new_figure(tetris_figure& shape);

    /* game_state_checkers/switchers */
    bool check_for_win() const;
    bool check_for_lose() const;

    void go_next_level();

    std::vector<tetris_shape> shapes_presets;
    std::vector<shape_type> shapes_types_presets;

    /*** Dynamic ***/

    // progress
  public:
    struct session_prog {
      int score{0};
      int total_lines{0};
      int figure_passed{0};
      int figure_passed_lvl{0};
    };
  private:
    session_prog prog;

    // difficulty
  public:
    struct session_diff {
      uint32_t level_number{1};
      int figure_to_win{10};
      uint32_t lose_line{2};
    };
    session_diff diff;
  private:
    state st{state::IDLE};

    tetris_field tetris_field;
    tetris_figure current_figure;
    tetris_figure next_figure;

    timer::seconds from_last_tick{0};

  public:
    struct session_stats {
      session_prog prog;
      session_diff diff;
    };

    struct metrics {
      int wins{0};
      int losses{0};
    } meta;

  private:
    std::vector<session_stats> played_sessions_stats;

  public:
    std::optional<session_stats> pop_last_played_session_stats();

    [[nodiscard]] session_prog get_prog() const noexcept {
      return prog;
    }

    [[nodiscard]] session_diff get_diff() const noexcept {
      return diff;
    }
  };
}
