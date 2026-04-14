/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <vector>

namespace tetris_bi {
  class tetris_field {
  public:
    [[nodiscard]] int get_cell(int x, int y) const;
    void change_cell(const int x, const int y, const int val);

    uint32_t get_width() const {
      return m_field_width;
    }

    uint32_t get_height() const {
      return m_field_height;
    }

    int clear_lines();

  private:

    void remove_line(const int row);
    bool is_line_full(const int row);

    const uint32_t m_field_width = 10;
    const uint32_t m_field_height = 20;
    std::vector<std::vector<int>> m_tetris_field_ =
      std::vector<std::vector<int>>(m_field_width, std::vector<int>(m_field_height));
  };

  enum class shape_type {
    O, I, S, Z, L, J, T
  };

  class tetris_game {
  public:
    tetris_game() {
      m_shapes_presets =
      {
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     1, 0}
        }, // O-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 + 1, 1, 0}
        }, // I-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2,     0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0}
        }, // S-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     1, 0}
        }, // Z-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2,     0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     1, 0}
        }, // L-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 1, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     1, 0}
        }, // J-figure
        {
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 2, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2,     0, 0},
          {static_cast<int>(m_tetris_field.get_width()) / 2 - 1, 1, 0}
        } // T-figure
      };

      m_shapes_types_presets = { shape_type::O, shape_type::I, shape_type::S, shape_type::Z,
                                 shape_type::L, shape_type::J, shape_type::T };

      generate_new_figure(m_current_figure);
      for (auto& p : m_current_figure.points) {
        m_tetris_field.change_cell(p.x, p.y, p.val);
      }
      generate_new_figure(m_next_figure);
    }

    const tetris_field& get_tetris_field() const {
      return m_tetris_field;
    }

    void move_direction(int dir);

    void rotateCW();
    void rotateCCW();

    void update();

  private:
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

    void generate_new_figure(tetris_figure& shape);

    std::vector<tetris_shape> m_shapes_presets;
    std::vector<shape_type> m_shapes_types_presets;

    tetris_field m_tetris_field;
    tetris_figure m_current_figure;
    tetris_figure m_next_figure;

    bool is_game_over = false;

    // stats
    int m_score = 0;
    int m_total_lines = 0;
    int m_figure_passed = 0;
  };
}