/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "super_puper_tetris_window.hpp"

#include "./tetris/tetris.hpp"

#include <iostream>
#include <format>

#include <chrono>
#include <thread>

namespace tetris_bi {

  super_puper_tetris_window::super_puper_tetris_window() :
    window("Nk1 av1 super tertis", 800, 800),
    render(800, 800)
  {
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);

    /* UI main menu */
    {
      auto button1 = std::make_unique<button>(20, 20, "Start manual tertis", MOUSE_BTN_1,
        [&]() {
          sc = scene::MANUAL_TETRIS;
          mt = std::make_unique<manual_tetris>();
        }
      );
      main_menu.add(std::move(button1));
      auto button2 = std::make_unique<button>(20, 60, "Start bots_client tertis choice", MOUSE_BTN_1,
        [&]() {
          sc = scene::BOTS_CLIENT_CHOICE;
        }
      );
      main_menu.add(std::move(button2));
    }

    /* UI bots_client_choice */
    {
      auto slider1 = std::make_unique<slider<uint32_t>>(20, 20,
        1, 100000, 200,
        500, MOUSE_BTN_1, "dummy bot number");
      slider_dummy = slider1.get();
      bots_client_choice.add(std::move(slider1));

      auto slider2 = std::make_unique<slider<uint32_t>>(20, 60,
        1, 100, 4,
        200, MOUSE_BTN_1, "clever bot number");
      slider_clever = slider2.get();
      bots_client_choice.add(std::move(slider2));

      auto button1 = std::make_unique<button>(20, 100, "Go", MOUSE_BTN_1,
        [&]() {
          sc = scene::BOTS_CLIENT;
          bsc = std::make_unique<bots_client>(slider_clever->get_value(), slider_dummy->get_value());
        }
      );
      bots_client_choice.add(std::move(button1));

      auto button2 = std::make_unique<button>(60, 100, "Back", MOUSE_BTN_1,
        [&]() {
          sc = scene::MAIN_MENU;
        }
      );
      bots_client_choice.add(std::move(button2));

      bots_client_choice.set_all_is_active_element(false);
    }

    /* Terminate */
    {
      auto button1 = std::make_unique<button>(20, 300, "Terminate", MOUSE_BTN_1,
        [&]() {
          sc = scene::MAIN_MENU;
          bsc = 0;
          mt = 0;
          draw_index = 0;
        }
      );
      terminate.add(std::move(button1));
      terminate.set_all_is_active_element(false);
    }
  }

  void super_puper_tetris_window::my_active(
    mfb_window*,
    [[maybe_unused]] bool is_active
  ) {
  }

  void super_puper_tetris_window::my_resize(
    mfb_window*,
    [[maybe_unused]] int w,
    [[maybe_unused]] int h
  ) {
    window::my_resize(nullptr, w, h);

    render::resize(w, h);
    render::draw_rectangle(0, 0, 100, 100, 0x834d18);
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }

  bool super_puper_tetris_window::my_close(
    mfb_window*
  ) {
    return true;
  }

  void super_puper_tetris_window::my_keyboard(
    mfb_window*,
    [[maybe_unused]] mfb_key key,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
    if (is_pressed) {
      if (sc == scene::MANUAL_TETRIS) {
        auto &game = mt->get_game();
        if (mod & mfb_key_mod::KB_MOD_CONTROL && key == mfb_key::KB_KEY_S) {
          game.start_session();
          return;
        }
        if (mod & mfb_key_mod::KB_MOD_CONTROL && key == mfb_key::KB_KEY_E) {
          game.end_session();
          return;
        }
        switch (key) {
          case mfb_key::KB_KEY_A:
            game.move_direction(-1);
            break;
          case mfb_key::KB_KEY_D:
            game.move_direction(1);
            break;
          case mfb_key::KB_KEY_S:
            game.rotateCW();
            break;
          case mfb_key::KB_KEY_W:
            game.rotateCCW();
            break;
          default:
            break;
        }
      } else if (sc == scene::BOTS_CLIENT) {
        auto bots_number = bsc->get_bots_number();
        switch (key) {
        case mfb_key::KB_KEY_D:
          draw_index = (draw_index + 1) % bots_number;
          break;
        case mfb_key::KB_KEY_A:
          draw_index = (draw_index - 1 + bots_number) % bots_number;
          break;
        default:
          break;
        }
      }
    }
  }

  void super_puper_tetris_window::my_char_input(
    mfb_window*,
    [[maybe_unused]] uint32_t char_code
  ) {
  }

  void super_puper_tetris_window::my_mouse_button(
    mfb_window*,
    [[maybe_unused]] mfb_mouse_button button,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] bool is_pressed
  ) {
  }

  void super_puper_tetris_window::my_mouse_move(
    mfb_window*,
    [[maybe_unused]] int mx,
    [[maybe_unused]] int my
  ) {
  }

  void super_puper_tetris_window::my_mouse_scroll(
    mfb_window*,
    [[maybe_unused]] mfb_key_mod mod,
    [[maybe_unused]] float mdx,
    [[maybe_unused]] float mdy
  ) {
  }

  void super_puper_tetris_window::my_frame() {
    fps_tim.update();
    beautiful_bg_273();

    update_keyboard(win);
    update_mbuttons(win);
    update_mpos(win, dpi_x, dpi_y);

    input_state is {
      .x = get_x(),
      .y = get_y(),
      .dx = get_dx(),
      .dy = get_dy(),
      .pressed_keys = get_pressed_keys().data(),
      .pressed_keys_instant = get_pressed_keys_instant().data()
    };
    main_menu.update(is);
    bots_client_choice.update(is);
    terminate.update(is);

    main_menu.draw(static_cast<render &>(*this));
    bots_client_choice.draw(static_cast<render &>(*this));
    terminate.draw(static_cast<render &>(*this));

    main_menu.set_all_is_active_element(sc == scene::MAIN_MENU);
    bots_client_choice.set_all_is_active_element(sc == scene::BOTS_CLIENT_CHOICE);
    terminate.set_all_is_active_element(sc == scene::MANUAL_TETRIS || sc == scene::BOTS_CLIENT);
    
    if (sc == scene::BOTS_CLIENT) {
      bsc->update();
      std::lock_guard guard(bsc->get_bots_mutex());
      const auto &target_tetris = bsc->get_bots()[draw_index]->game;

      render_tetris(target_tetris.get_tetris_field());
      auto prog = target_tetris.get_prog(); auto diff = target_tetris.get_diff();

      auto str1 = std::format(R"(Nk1 av1 super tertis, FPS: {})", fps_tim.fps);
      auto str2 = std::format(R"(Bot index: {})", draw_index);
      auto str3 = std::format(R"(Total bots: {})", bsc->get_bots_number());
      auto str4 = std::format(
        R"(Tertis prog score               : {}
Tertis prog total_lines         : {},
Tertis prog figure_passed       : {},
Tertis prog figure_passed_lvl   : {},
Tertis diff level_number        : {},
Tertis diff figure_to_win       : {},
Tertis diff lose_line:          : {},
        )", prog.score, prog.total_lines, prog.figure_passed, prog.figure_passed_lvl,
            diff.level_number, diff.figure_to_win, diff.lose_line);
            auto str5 = std::format("Connected: {}", bsc->get_connection_status() ? "true" : "false");

      draw_string(str1, 0, 0, 0x834d18);
      draw_string(str3, 0, 16, 0x006600);
      draw_string(str2, 0, 32, 0x660066);
      draw_string(str5, 0, 48, bsc->get_connection_status() ? 0x00FF00 : 0xFF0000);
      draw_string(str4, 0, 64, 0x666666);
    } else if (sc == scene::MANUAL_TETRIS) {
      mt->update();
      const auto &target_tetris = mt->get_game();
      render_tetris(target_tetris.get_tetris_field());

      auto prog = target_tetris.get_prog(); auto diff = target_tetris.get_diff();

      auto str1 = std::format(R"(Nk1 av1 super tertis, FPS: {})", fps_tim.fps);
      auto str4 = std::format(
        R"(Tertis prog score               : {}
Tertis prog total_lines         : {},
Tertis prog figure_passed       : {},
Tertis prog figure_passed_lvl   : {},
Tertis diff level_number        : {},
Tertis diff figure_to_win       : {},
Tertis diff lose_line:          : {},
        )", prog.score, prog.total_lines, prog.figure_passed, prog.figure_passed_lvl,
            diff.level_number, diff.figure_to_win, diff.lose_line);
            auto str5 = std::format("Connected: {}", mt->get_connection_status() ? "true" : "false");

      draw_string(str1, 0, 0, 0x834d18);
      draw_string("LeftCtrl+S start session LeftCtrl+E end session", 0, 16, 0xFFFFFF);

      draw_string(str5, 0, 48, mt->get_connection_status() ? 0x00FF00 : 0xFF0000);
      draw_string(str4, 0, 64, 0x666666);
    }
    mfb_update_ex(win, render::get_buffer().data(), window::width, window::height);
  }
}
