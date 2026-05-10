/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include <cstdint>
#include <string>

#include "render.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

namespace tetris_bi {
  struct input_state {
    int x, y;
    int dx, dy;
    const bool
      *pressed_keys,
      *pressed_keys_instant;
  };

  class ui_element {
    friend class ui;
  protected:
    bool is_active_element{true};
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
      for (auto &e : ui_elements) if (e->is_active_element) e->draw(rnd);
    }

    void add(std::unique_ptr<ui_element> e) {
      ui_elements.push_back(std::move(e));
    }

    void update(const input_state &is) {
      for (auto &e : ui_elements) if (e->is_active_element) e->update(is);
    }

    void set_all_is_active_element(bool active) {
      for (auto &e : ui_elements) e->set_is_active_element(active);
    }
  };

  class button : public ui_element {
  private:
    int x1, y1, x2, y2;
    std::string text;
    uint32_t trigger_key;
    bool is_hovered{false};
    bool is_hold{false};
    bool is_pressed{false};
    static constexpr uint32_t MARGIN_X = 10, MARGIN_Y = 4;
    std::function<void()> f;

    void draw(render &rnd) override;
    void update(const input_state &state) override;

  public:
    button(int x1, int y1, std::string text, uint32_t trigger_key, const std::function<void()> &f = [](){});
  };

  template<typename T>
  class slider : public ui_element {
  private:
    T a, b;
    int x1, y1;
    uint32_t length;
    double one_interval;
    int shaft_left_x;
    int shaft_x, shaft_y;
    static constexpr uint32_t SHAFT_WIDTH = 10, SHAFT_HEIGHT = 20, BAR_SIZE = 6;
    uint32_t trigger_key;
    std::string comment;
    T value;

    struct {
      bool is_hovered{false};
      bool is_hold{false};
      bool is_pressed{false};
    } bar, shaft;

    bool shaft_was_pressed{false};

    void draw(render &rnd) override {
      const uint32_t bar_color = bar.is_hovered ? (bar.is_hold ? 0xAAAAAA : 0x777777) : 0x444444;
      const uint32_t shaft_color = shaft_was_pressed ? 0x834D18 : (shaft.is_hovered ? 0xAAAAAA : 0x777777);

      rnd.draw_rectangle(x1, y1 - BAR_SIZE / 2, x1 + length, y1 + BAR_SIZE / 2, bar_color);
      rnd.draw_rectangle(shaft_x, shaft_y, shaft_x + SHAFT_WIDTH, shaft_y + SHAFT_HEIGHT, shaft_color);

      auto ty = y1 + BAR_SIZE + 10;

      std::string text = std::format("Slider ^ value ({}): {}", comment, value);
      rnd.draw_rectangle(x1, ty, x1 + text.size() * rnd.FONT_W, ty + rnd.FONT_H, 0);
      rnd.draw_string(text, x1, ty, 0xFFFFFF);
      
    }

    void update(const input_state &state) override {
      bar = {}; shaft = {};

      if (!state.pressed_keys[trigger_key]) {
        shaft_was_pressed = false;
      }

      if (shaft_x <= state.x && state.x <= shaft_x + SHAFT_WIDTH &&
          shaft_y <= state.y && state.y <= shaft_y + SHAFT_HEIGHT) {
        shaft.is_hovered = true;

        shaft.is_hold = state.pressed_keys[trigger_key];
        shaft.is_pressed = state.pressed_keys_instant[trigger_key];

        if (state.pressed_keys_instant[trigger_key]) {
          shaft_was_pressed = true;
        }
      } else if (x1 <= state.x && state.x <= x1 + length &&
                 y1 - BAR_SIZE <= state.y && state.y <= y1 + BAR_SIZE) {
        bar.is_hovered = true;

        bar.is_hold = state.pressed_keys[trigger_key];
        bar.is_pressed = state.pressed_keys_instant[trigger_key];
      }

      auto want = shaft_x + state.dx;
      if (shaft_was_pressed && shaft_left_x <= want && want <= shaft_left_x + length) {
        shaft_x = want;
        value = static_cast<T>(a + (shaft_x - shaft_left_x) * one_interval);
      }

      if (bar.is_pressed) {
        shaft_x = shaft_left_x + state.x - x1;
        value = static_cast<T>(a + (shaft_x - shaft_left_x) * one_interval);
      }

    }

  public:
    slider(int x1, int y1, T a, T b, T start,
      uint32_t length, uint32_t trigger_key, const std::string &comment = "") :
      x1(x1), y1(y1),
      shaft_left_x(x1 - SHAFT_WIDTH / 2),
      shaft_y(y1 - SHAFT_HEIGHT / 2),
      a(a), b(b),
      length(length),
      trigger_key(trigger_key),
      value(start),
      one_interval((b - a) / (double)length),
      comment(comment),
      shaft_x(shaft_left_x + (start - a) / (double)(b - a) * length)
    {
    }

    T get_value() const {
      return value;
    }
  };
}
