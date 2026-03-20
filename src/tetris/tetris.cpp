/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include <cstdlib>
#include <cstdint>

#include "tetris.hpp"

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

/* tetris_game */

[[nodiscard]] bool tetris_game::check_current_shape(const int x, const int y) const {
  for (auto& p : m_current_shape) {
    if (p.x == x && p.y == y) {
      return true;
    }
  }
  return false;
}

[[nodiscard]] bool tetris_game::check_down() const {
  for (auto& p : m_current_shape) {
    bool is_yourself = check_current_shape(p.x, p.y + 1);
    if (m_tetris_field.get_cell(p.x, p.y + 1) != 0 && !is_yourself) {
      return false;
    }
  }
  return true;
}

void tetris_game::move_down() {
  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, 0);
    p.y++;
  }
  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, p.val);
  }
}

[[nodiscard]] int tetris_game::get_direction(direction dir) const {
  switch (dir) {
  case direction::right:
    return 1;
  case direction::left:
    return -1;
  }
}

[[nodiscard]] bool tetris_game::check_direction(direction dir) const {
  int d = get_direction(dir);

  for (auto& p : m_current_shape) {
    bool is_yourself = check_current_shape(p.x + d, p.y);
    if (m_tetris_field.get_cell(p.x + d, p.y) != 0 && !is_yourself) {
      return false;
    }
  }
  return true;
}

void tetris_game::move_direction(direction dir) {
  int d = get_direction(dir);

  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, 0);
    p.x += d;
  }
  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, p.val);
  }
}

bool tetris_game::check_rotateCW() const {
  point rotate_point = m_current_shape[1];

  for (auto& p : m_current_shape) {
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
  point rotate_point = m_current_shape[1];

  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, 0);
    int new_x = p.x - rotate_point.x;
    int new_y = p.y - rotate_point.y;
    std::swap(new_x, new_y);
    new_y = -new_y;
    p.x = new_x + rotate_point.x;
    p.y = new_y + rotate_point.y;
  }
  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, p.val);
  }
}

bool tetris_game::check_rotateCCW() const {
  point rotate_point = m_current_shape[2];

  for (auto& p : m_current_shape) {
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
  point rotate_point = m_current_shape[1];

  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, 0);
    int new_x = p.x - rotate_point.x;
    int new_y = p.y - rotate_point.y;
    std::swap(new_x, new_y);
    new_x = -new_x;
    p.x = new_x + rotate_point.x;
    p.y = new_y + rotate_point.y;
  }
  for (auto& p : m_current_shape) {
    m_tetris_field.change_cell(p.x, p.y, p.val);
  }
}

void tetris_game::generate_new_shape(tetris_shape& shape) {
  int figure_id = rand() % 7; // BAD Random
  shape = m_shapes[figure_id];
  int random_val = rand() % 8 + 1; // BAD Random
  for (auto& p : shape) {
    p.val = random_val;
  }
}

void tetris_game::update() {
  if (is_game_over) return;
  
  // int rand_dir = rand() % 2;
  // if (rand_dir == 0 && check_direction(direction::right)) move_direction(direction::right);
  // else if (rand_dir == 1 && check_direction(direction::left)) move_direction(direction::left);
  static int counter = 0;
  if (counter++ < 3) move_down();
  else if (check_rotateCW()) rotateCW();
  /*
  if (check_down()) {
    move_down();
  } else {
    m_current_shape = m_next_shape;
    generate_new_shape(m_next_shape);
    return;
  }
  */
}
