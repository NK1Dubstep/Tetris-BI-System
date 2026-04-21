/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

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

    const uint32_t m_field_width{10};
    const uint32_t m_field_height{20};
    std::vector<std::vector<int>> m_tetris_field_ =
      std::vector<std::vector<int>>(m_field_width, std::vector<int>(m_field_height));
  };

  enum class shape_type {
    O, I, S, Z, L, J, T
  };

  class tetris_game {
  public:
    tetris_game();

    ~tetris_game() {
      is_game_over = true;
    }

    const tetris_field& get_tetris_field() const {
      return m_tetris_field;
    }

    void move_direction(int dir);

    void rotateCW();
    void rotateCCW();

    void update(timer::seconds delta_time);

  private:
    void generate_shapes_data();
    void tick();

    enum class state : short {
      IDLE, SESSION
    };
    state st{state::SESSION};

    struct point {
      point(int x_ = 0, int y_ = 0, int val_ = 0) : x(x_), y(y_), val(val_) {}

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

    [[nodiscard]] bool check_down() const;
    void move_down();

    [[nodiscard]] bool check_direction(int dir) const;

    [[nodiscard]] bool check_rotateCW() const;
    [[nodiscard]] bool check_rotateCCW() const;

    void commit_current_figure();
    void generate_new_figure(tetris_figure& shape);

    /* game_state_checkers/switchers */
    bool check_for_win() const;
    bool check_for_lose() const;

    void go_next_level();

    int m_figure_to_win = 500;
    uint32_t m_level_number = 1;
    uint32_t m_lose_line = 2;
    int m_figure_passed_lvl = 0;

    std::vector<tetris_shape> m_shapes_presets;
    std::vector<shape_type> m_shapes_types_presets;

    tetris_field m_tetris_field;
    tetris_figure m_current_figure;
    tetris_figure m_next_figure;

    bool is_game_over = false;

    timer::seconds from_last_tick{0};

    // stats
    int m_score = 0;
    int m_total_lines = 0;
    int m_figure_passed = 0;
  };
}
