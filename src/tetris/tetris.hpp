/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <vector>

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

private:

  const uint32_t m_field_width = 10;
  const uint32_t m_field_height = 20;
  std::vector<std::vector<int>> m_tetris_field_ =
    std::vector<std::vector<int>>(m_field_width, std::vector<int>(m_field_height));
};

class tetris_game {
public:
  tetris_game() {
    m_shapes =
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

    generate_new_shape(m_current_shape);
    for (auto& p : m_current_shape) {
      m_tetris_field.change_cell(p.x, p.y, p.val);
    }
    generate_new_shape(m_next_shape);
  }

  void update();

  const tetris_field& get_tetris_field() const {
    return m_tetris_field;
  }
private:
  struct point {
    point(int x_, int y_, int val_) : x(x_), y(y_), val(val_) {}
    int x = 0;
    int y = 0;
    int val = 0;
  };

  enum class direction {
    right,
    left
  };

  using tetris_shape = std::vector<point>;

  [[nodiscard]] bool check_current_shape(const int x, const int y) const;

  [[nodiscard]] bool check_down() const;
  void move_down();
  
  [[nodiscard]] int get_direction(direction dir) const;

  [[nodiscard]] bool check_direction(direction dir) const;
  void move_direction(direction dir);

  bool check_rotateCW() const;
  bool check_rotateCCW() const;
  void rotateCW();
  void rotateCCW();

  void generate_new_shape(tetris_shape& shape);

  std::vector<tetris_shape> m_shapes;

  tetris_field m_tetris_field;
  tetris_shape m_current_shape;
  tetris_shape m_next_shape;

  bool is_game_over = false;
};