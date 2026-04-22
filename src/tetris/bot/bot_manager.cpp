/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "bot_manager.hpp"
#include "utils/random.hpp"

#include <optional>
#include <format>

namespace tetris_bi {

  bot_manager::bot_manager(tetris_game *games, size_t games_number,
    timer::seconds tick_interval) :
      m_games(games),
      m_games_number(games_number),
      m_client("127.0.0.1", 5837),
      m_tick_interval(tick_interval)
  {
    static constexpr timer::seconds TIMEOUT = 2;
    m_client.set_connection_timeout(TIMEOUT);

    auto res = m_client.Get("/tetris_bi");
    is_connected = res && res->status == 200;

    std::cout << std::format("Is connected in {} seconds: {}\n", TIMEOUT, is_connected);

    m_bots.reserve(games_number);
    for (size_t i = 0; i < games_number; i++) {
      m_bots.push_back(bot(m_games[i]));
    }

    m_last_tick_times.resize(m_games_number, m_timer.time_p);
  }

  void bot_manager::run_bots(std::atomic<bool> *to_stop, std::mutex *mt) {
    while (!*to_stop) {
      {
        std::unique_lock<std::mutex> lock;
        if (mt) {
          lock = std::unique_lock<std::mutex>(*mt);
        }
        m_timer.update();
        auto dt = m_timer.delta_time_p;
        auto tp = m_timer.time_p;

        for (int i = 0; i < m_games_number; i++) {
          m_games[i].update(dt);
          if (tp - m_last_tick_times[i] > m_tick_interval) {
            m_bots[i].make_move();
            m_last_tick_times[i] = tp;
          }
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}
