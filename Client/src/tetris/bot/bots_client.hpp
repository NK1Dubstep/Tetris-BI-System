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

namespace tetris_bi {
  class bots_client : private stats_sender {
    friend class super_tetris_window;
  public:
    bots_client(uint32_t bots_number);
    ~bots_client() { disconnect(); };

    void update();

  private:
    int bots_number;

    struct bot {
      std::optional<uint32_t> id{std::nullopt};
      tetris_game game;
      tetris_game::state prev_state{tetris_game::state::IDLE};
      timer::seconds in_session{0};
      timer::seconds in_idle{0};
      timer::seconds session_exit, idle_exit;
      timer::seconds last_tick{0};
      static constexpr timer::seconds TICK_INTERVAL{0.1};
      int wins{0};
      int losses{0};
      int max_streak{0};
    };

    std::atomic<bool> register_finished{false};
    std::vector<bot> bots;
    std::mutex bots_mutex;
    timer tim;

    void on_open() override;
    void on_close() override;
    void on_message(const nlohmann::json &data) override;
    nlohmann::json on_update_metrics() override;
  };
}

