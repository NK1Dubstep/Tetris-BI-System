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

#include <queue>
#include <unordered_map>

namespace tetris_bi {
  class bots_client {
    friend class super_tetris_window;
  public:
    bots_client(int n);
    ~bots_client() {ws.stop();}

    void update();

  private:
    int number;

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

    std::unordered_map<uint32_t, bool> is_playing_tetris_updates;
    timer::seconds last_send_time;

    uint32_t played_sessions = 0;

    ix::WebSocket ws;
    std::atomic<bool> is_connected;

    void connect();

    void bots_register();
    std::atomic<bool> register_finished{false};

    timer tim;
    std::vector<bot> bots;
    std::mutex bots_mutex;
  };
}

