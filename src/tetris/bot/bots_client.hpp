/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "utils/random.hpp"

#include <queue>

namespace tetris_bi {
  class bots_client {
    friend class super_tetris_window;
  public:
    bots_client(int n);

    void update(timer::seconds delta_time);

  private:
    struct bot {
      tetris_game game;
      tetris_game::state prev_state{tetris_game::state::IDLE};
      timer::seconds in_session{0};
      timer::seconds in_idle{0};
      timer::seconds session_exit, idle_exit;
      timer::seconds last_tick{0};
      static constexpr timer::seconds TICK_INTERVAL{0.1};
    };

    std::vector<bot> bots;
  };
}
