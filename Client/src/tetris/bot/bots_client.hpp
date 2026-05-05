/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

// IXWebSocket
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "utils/random.hpp"
#include "tetris/stats_sender/stats_sender.hpp"

#include <queue>
#include <unordered_map>
#include <memory>
#include <optional>

namespace tetris_bi {
  class bots_client : private stats_sender {
    friend class super_tetris_window;
  public:
    bots_client(uint32_t clever_bot_number, uint32_t dummy_bot_number);
    ~bots_client() { disconnect(); };
    
    void update();

  private:
    int bots_number{0};

    struct bot {
      bot() = default;
      bot(timer::seconds tick_interval) {
        TICK_INTERVAL = tick_interval;
      }

      std::optional<uint32_t> id{std::nullopt};
      tetris_game game;
      tetris_game::state prev_state{tetris_game::state::IDLE};
      timer::seconds in_session{0};
      timer::seconds in_idle{0};
      timer::seconds session_exit{1}, idle_exit{1};
      timer::seconds last_tick{0};
      timer::seconds TICK_INTERVAL{0.1};

      virtual void make_move() {
        uint32_t move_id = generate_randui32() % 5;
        switch (move_id) {
        case 0: game.move_direction(1); break;
        case 1: game.move_direction(-1); break;
        case 2: game.rotateCW(); break;
        case 3: game.rotateCCW(); break;
        default: break;
        }
      }
    };

    struct bot_200iq : public bot {
      // bot_200iq() = default;
      bot_200iq(timer::seconds tick_interval = 0.01) {
        TICK_INTERVAL = tick_interval;
      }

      std::optional<int> best_rotate{std::nullopt};
      std::optional<int> best_move{std::nullopt};
      int curr_figure_passed = -1;

      int count_iso_cells(const tetris_game& game);
      int eval(const tetris_game& game);
      void calculate_best_way();
      void make_move() override;
    };

    std::atomic<bool> register_finished{false};
    std::vector<std::unique_ptr<bot>> bots;
    std::mutex bots_mutex;
    timer tim;

    void ss_on_open() override;
    void ss_on_close() override;
    void ss_on_message(const nlohmann::json &data) override;
    nlohmann::json ss_on_update_metrics() override;
  };
}
