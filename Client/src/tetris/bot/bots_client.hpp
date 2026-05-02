/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

// IXWebSocket
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "utils/random.hpp"

#include <queue>

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

    struct is_playing_tetris_update {
      uint32_t id;
      bool value;
    };

    std::vector<is_playing_tetris_update> is_playing_tetris_updates;
    timer::seconds last_iptu_send_time;

    uint32_t played_sessions = 0;

    ix::WebSocket ws;
    std::atomic<bool> is_connected;

    void connect();

    void bot_register(int i);
    void bots_register();
    std::atomic<bool> register_finished{false};
    void set_is_playing_tetris(bot &bt, bool b);

    timer tim;
    std::vector<bot> bots;
    std::mutex bots_mutex;
  };
}
