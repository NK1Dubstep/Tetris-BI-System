/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "tetris/tetris.hpp"
#include "bots_client.hpp"

#include "nlohmann/json.hpp"

#include <iostream>

namespace tetris_bi {

  namespace {
    auto generate_random_idle_exit() {
      return generate_randui32() % 5;
    }

    auto generate_random_session_exit() {
      return 1000;  // generate_randui32() % 60 + 90;
    }
  }

  bots_client::bots_client(uint32_t bots_number) :
    stats_sender{}, bots(bots_number), bots_number(bots_number)
  {
    for (auto &bot : bots) {
      bot.idle_exit = generate_random_idle_exit();
      bot.session_exit = generate_random_session_exit();
    }
    connect(5);
  }

  void bots_client::update() {
    if (!register_finished) {
      return;
    }
    std::lock_guard guard(bots_mutex);

    tim.update();
    auto dt = tim.delta_time_p;
    auto t = tim.time_p;

    for (auto &bot : bots) {
      auto stats = bot.game.pop_last_played_session_stats();

      tetris_game::state st;
      bot.game.update(dt);
      st = bot.game.get_state();

      if (st == tetris_game::state::IDLE) {
        if (bot.prev_state == tetris_game::state::SESSION) {
          if (bot.id.has_value()) {
            set_is_playing_tetris_update(bot.id.value(), false);
          }
          bot.idle_exit = generate_random_idle_exit();
          bot.in_idle = 0;
          bot.max_streak = max(bot.max_streak, bot.game.diff.level_number - 1);
        } else {
          bot.in_idle += dt;
        }
        if (bot.in_idle > bot.idle_exit) {
          bot.game.start_session();
        }
        bot.prev_state = tetris_game::state::IDLE;
      } else if (st == tetris_game::state::SESSION) {
        if (bot.prev_state == tetris_game::state::IDLE) {
          if (bot.id.has_value()) {
            set_is_playing_tetris_update(bot.id.value(), true);
            increase_sessions();
          }
          bot.session_exit = generate_random_session_exit();
          bot.in_session = 0;
          bot.last_tick = 0;
        } else {
          bot.in_session += dt;
          bot.last_tick += dt;
        }
        if (bot.in_session > bot.session_exit) {
          bot.game.end_session();
        } else if (bot.last_tick > bot.TICK_INTERVAL) {
          uint32_t move_id = generate_randui32() % 5;
          switch (move_id) {
            case 0: bot.game.move_direction(1); break;
            case 1: bot.game.move_direction(-1); break;
            case 2: bot.game.rotateCW(); break;
            case 3: bot.game.rotateCCW(); break;
            default: break;
          }
          bot.last_tick = 0;
        }
        bot.prev_state = tetris_game::state::SESSION;
      }
    }  // end of for on bots

    flush_update(t);
  }

  void bots_client::on_open() {
    register_batch(bots_number);
  }

  void bots_client::on_close() {
    register_finished = false;
    for (auto &b : bots) b = {};
  }

  void bots_client::on_message(const nlohmann::json &data) {
    std::string message = data["message"];

    if (message == "register_batch") {
      auto x = data["ids"];

      std::lock_guard guard(bots_mutex);
      for (int i = 0; i < bots_number; i++) {
        bots[i].id = x[i];
      }
      register_finished = true;
    }
  }

  nlohmann::json bots_client::on_update_metrics() {
    nlohmann::json arr = nlohmann::json::array();
    auto &vec = arr.get_ref<nlohmann::json::array_t&>();
    vec.reserve(bots.size());

    for (auto& bot : bots) {
      if (bot.id.has_value()) {
        bot.max_streak = max(bot.max_streak, bot.game.diff.level_number - 1);
        arr.push_back({
          {"id", bot.id},
          {"wins", bot.game.meta.wins - bot.wins},
          {"losses", bot.game.meta.losses - bot.losses},
          {"max_streak", bot.max_streak}
          });
      }
      bot.wins = bot.game.meta.wins;
      bot.losses = bot.game.meta.losses;
      bot.max_streak = 0;
    }
    return arr;
  }
}
