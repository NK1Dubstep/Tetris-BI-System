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

  bots_client::bots_client(int clever_bot_number, int dummy_bot_number) :
    bots(clever_bot_number + dummy_bot_number),
    number(clever_bot_number + dummy_bot_number)
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
    connect();
    is_playing_tetris_updates.reserve(n);
    last_send_time = tim.time;
  }

  void bots_client::update() {
    // if (!register_finished) {
    //   return;
    // }
    std::lock_guard guard(bots_mutex);

    tim.update();
    auto dt = tim.delta_time_p;

    for (auto &bot : bots) {
      auto stats = bot->game.pop_last_played_session_stats();

      tetris_game::state st;
      bot->game.update(dt);
      st = bot->game.get_state();
      if (st == tetris_game::state::IDLE) {
        if (bot->prev_state == tetris_game::state::SESSION) {
          if (bot->id.has_value()) {
            is_playing_tetris_updates[bot->id.value()] = false;
          }
          bot->idle_exit = generate_random_idle_exit();
          bot->in_idle = 0;
          bot->max_streak = max(bot->max_streak, bot->game.diff.level_number - 1);
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
            is_playing_tetris_updates[bot->id.value()] = true;
            played_sessions += 1;
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
    }  // end of for

    if (is_connected && tim.time - last_send_time > 1) {
      nlohmann::json arr = nlohmann::json::array();
      auto &vec = arr.get_ref<nlohmann::json::array_t&>();
      vec.reserve(is_playing_tetris_updates.size());

      for (auto &[id, value] : is_playing_tetris_updates) {
        arr.push_back({{"id", id}, {"value", value}});
      }
      ws.send(nlohmann::json{
        {"type", "set_is_playing_tetris_batch"},
        {"data", arr}
        }.dump());
      is_playing_tetris_updates.clear();

      ws.send(nlohmann::json{
        {"type", "increase_number_of_sessions"},
        {"data", played_sessions}
        }.dump());
      played_sessions = 0;

      nlohmann::json arr_wl = nlohmann::json::array();
      auto& vec_wl = arr_wl.get_ref<nlohmann::json::array_t&>();
      vec_wl.reserve(bots.size());
      for (auto& bot : bots) {
        if (bot->id.has_value()) {
          bot->max_streak = max(bot->max_streak, bot->game.diff.level_number - 1);
          arr_wl.push_back({
            {"id", bot->id},
            {"wins", bot->game.meta.wins - bot->wins},
            {"losses", bot->game.meta.losses - bot->losses},
            {"max_streak", bot->max_streak}
            });
        }
        bot->wins = bot->game.meta.wins;
        bot->losses = bot->game.meta.losses;
        bot->max_streak = 0;
      }
      ws.send(nlohmann::json{
        {"type", "update_metrics"},
        {"data", arr_wl}
        }.dump());

      last_send_time = tim.time;
    }
  }

  void bots_client::connect() {
    ws.setUrl("ws://127.0.0.1:5837/ws");

    ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
      if (msg->type == ix::WebSocketMessageType::Open) {
        std::cout << "WebSocket connected!" << std::endl;
        is_connected = true;
        bots_register();
      }
      else if (msg->type == ix::WebSocketMessageType::Close) {
        std::cout << "WebSocket closed" << std::endl;
        is_connected = false;
        register_finished = false;
        for (auto &b : bots) b = {};
      }
      else if (msg->type == ix::WebSocketMessageType::Message) {
        nlohmann::json js = nlohmann::json::parse(msg->str);
        std::string message = js["message"];

        if (message == "register_batch") {
          auto x = js["ids"];

          std::lock_guard guard(bots_mutex);

          for (int i = 0; i < number; i++) {
            bots[i]->id = x[i];
          }
          register_finished = true;
        }
      }
      else if (msg->type == ix::WebSocketMessageType::Error) {
        std::cerr << "WebSocket error: " << msg->errorInfo.reason << std::endl;
      }
    });

    ws.start();

    for (int i = 0; i < 50 && !is_connected; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (!is_connected) {
      std::cout << "Bots client: websocket is not connected in 5 seconds. Fail\n";
      ws.stop();
    }
  }

  void bots_client::bots_register() {
    if (is_connected) {
      ws.send(R"({"type":"register_batch", "number": )" + std::to_string(number) + "}");
    }
  }
}
