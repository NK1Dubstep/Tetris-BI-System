/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "tetris/tetris.hpp"
#include "manual_tetris.hpp"

#include "nlohmann/json.hpp"

#include <iostream>

namespace tetris_bi {

  manual_tetris::manual_tetris() :
    stats_sender{}
  {
    connect(0.5);
  }

  void manual_tetris::update() {
    tim.update();

    auto dt = tim.delta_time_p;
    auto t = tim.time_p;

    game.update(dt);

    tetris_game::state st{game.get_state()};

    if (st == tetris_game::state::IDLE) {
      if (prev_state == tetris_game::state::SESSION) {
        if (id.load().has_value()) {
          set_is_playing_tetris_update(id.load().value(), false);
        }
        increase_sessions();
      }
      prev_state = tetris_game::state::IDLE;
    } else if (st == tetris_game::state::SESSION) {
      if (prev_state == tetris_game::state::IDLE) {
        if (id.load().has_value()) {
          set_is_playing_tetris_update(id.load().value(), true);
        }
      }
      prev_state = tetris_game::state::SESSION;
    }

    stats_sender::flush_update(t);
  }

  void manual_tetris::ss_on_open() {
    register_batch(1);
  }

  void manual_tetris::ss_on_close() {
    id = std::nullopt;
  }

  void manual_tetris::ss_on_message(const nlohmann::json &data) {
    std::string message = data["message"];

    if (message == "register_batch") {
      auto x = data["ids"];
      id = x[0];
    }
  }

  nlohmann::json manual_tetris::ss_on_update_metrics() {
    nlohmann::json arr = nlohmann::json::array();

    if (id.load().has_value()) {
      tetris_game::metrics dmeta = game.get_meta_deltas();

      arr.push_back({
        {"id", id.load().value()},
        {"wins", dmeta.wins},
        {"losses", dmeta.losses},
        {"max_streak", dmeta.max_streak}
        });
    }
    game.save_meta_accum();
    return arr;
  }
}
