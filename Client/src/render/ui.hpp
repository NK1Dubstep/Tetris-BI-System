/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include <cstdint>
#include <string>

#include "render.hpp"
#include <vector>
#include <memory>
#include <iostream>

namespace tetris_bi {
  struct input_state {
    int x, y;
    const bool
      *pressed_keys,
      *pressed_keys_instant;
  };

  class ui_element {
    friend class ui;
  protected:
    bool is_active_element;
    virtual void update(const input_state &state) = 0;
    virtual void draw(render &rnd) = 0;

  public:
    void set_is_active_element(bool active) {is_active_element = active;}

    virtual ~ui_element() {};

  };

  class ui {

  private:
    std::vector<std::unique_ptr<ui_element>> ui_elements;

  public:
    void draw(render &rnd) {
      for (auto &e : ui_elements) e->draw(rnd);
    }

    void add(std::unique_ptr<ui_element> e) {
      ui_elements.push_back(std::move(e));
    }

    void update(const input_state &is) {
      for (auto &e : ui_elements) e->update(is);
    }
  };

  class button : public ui_element {
  private:
    uint32_t x1, y1, x2, y2;
    std::string text;
    uint32_t trigger_key;
    bool is_hovered{false};
    bool is_hold{false};
    bool is_pressed{false};
    static constexpr uint32_t MARGIN_X = 10, MARGIN_Y = 4;


    void draw(render &rnd) override;
    void update(const input_state &state) override;

  public:
    button(uint32_t x1, uint32_t y1, std::string text, uint32_t trigger_key);
  };
}
