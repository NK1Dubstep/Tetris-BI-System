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

  bots_client::bots_client(uint32_t clever_bot_number, uint32_t dummy_bot_number) :
    stats_sender{},
    bots(clever_bot_number + dummy_bot_number),
    bots_number(clever_bot_number + dummy_bot_number)
  {
    int n = clever_bot_number + dummy_bot_number;
    for (int i = 0; i < clever_bot_number; i++) {
      bots[i] = std::make_unique<bot_200iq>();
    }

    for (int i = clever_bot_number; i < n; i++) {
      bots[i] = std::make_unique<bot>();
    }

    for (auto &bot : bots) {
      bot->idle_exit = generate_random_idle_exit();
      bot->session_exit = generate_random_session_exit();
    }
    connect(0.5);
  }

  void bots_client::update() {
    // if (!register_finished) {
    //   return;
    // }
    std::lock_guard guard(bots_mutex);

    tim.update();
    auto dt = tim.delta_time_p;
    auto t = tim.time_p;

    for (auto &bot : bots) {
      tetris_game::state st;
      bot->game.update(dt);
      st = bot->game.get_state();

      if (st == tetris_game::state::IDLE) {
        if (bot->prev_state == tetris_game::state::SESSION) {
          if (bot->id.has_value()) {
            set_is_playing_tetris_update(bot->id.value(), false);
          }
          increase_sessions();
          bot->idle_exit = generate_random_idle_exit();
          bot->in_idle = 0;
        } else {
          bot->in_idle += dt;
        }
        if (bot->in_idle > bot->idle_exit) {
          bot->game.start_session();
        }
        bot->prev_state = tetris_game::state::IDLE;
      } else if (st == tetris_game::state::SESSION) {
        if (bot->prev_state == tetris_game::state::IDLE) {
          if (bot->id.has_value()) {
            set_is_playing_tetris_update(bot->id.value(), true);
          }
          bot->session_exit = generate_random_session_exit();
          bot->in_session = 0;
          bot->last_tick = 0;
        } else {
          bot->in_session += dt;
          bot->last_tick += dt;
        }
        if (bot->in_session > bot->session_exit) {
          bot->game.end_session();
        } else if (bot->last_tick > bot->TICK_INTERVAL) {
          bot->make_move();
          bot->last_tick = 0;
        }
        bot->prev_state = tetris_game::state::SESSION;
      }
    }  // end of for on bots

    flush_update(t);
  }

  void bots_client::ss_on_open() {
    register_batch(bots_number);
  }

  void bots_client::ss_on_close() {
    register_finished = false;
    for (auto &b : bots) b->reset();
  }

  void bots_client::ss_on_message(const nlohmann::json &data) {
    std::string message = data["message"];

    if (message == "register_batch") {
      auto x = data["ids"];

      std::lock_guard guard(bots_mutex);
      for (int i = 0; i < bots_number; i++) {
        bots[i]->id = x[i];
      }
      register_finished = true;
    }
  }

  nlohmann::json bots_client::ss_on_update_metrics() {
    nlohmann::json arr = nlohmann::json::array();
    auto &vec = arr.get_ref<nlohmann::json::array_t&>();
    vec.reserve(bots.size());

    for (auto &bot : bots) {
      if (bot->id.has_value()) {
        tetris_game::metrics dmeta = bot->game.get_meta_deltas();

        arr.push_back({
          {"id", bot->id.value()},
          {"wins", dmeta.wins},
          {"losses", dmeta.losses},
          {"max_streak", dmeta.max_streak}
          });
      }
      bot->game.save_meta_accum();
    }
    return arr;
  }
}
