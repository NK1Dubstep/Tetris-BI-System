/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <vector>
#include <mutex>
#include <optional>

#include "bot.hpp"
#include "httplib.h"
#include "tetris/tetris.hpp"
#include "utils/timer.hpp"


namespace tetris_bi {

  class bot_manager {
  private:
    std::vector<bot> m_bots;
    timer::seconds m_tick_interval;
    std::vector<timer::seconds> m_last_tick_times;
    tetris_game *m_games;
    size_t m_games_number;
    httplib::Client m_client;
    bool is_connected;
    timer m_timer;

  public:
    bot_manager(tetris_game *games, size_t games_number,
      timer::seconds tick_interval);

    void run_bots(std::atomic<bool> *to_stop, std::mutex *mt = nullptr);

  };
}

