/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include "tetris/tetris.hpp"
#include "utils/random.hpp"

namespace tetris_bi {
  class bot {
  public:
    bot(tetris_game& game_);

    uint32_t get_id() { return m_bot_id; }

    void make_move();
    bool send_stats();

  private:
    uint32_t m_bot_id = 0;

    int score = 0;
    int level = 0;

    tetris_game& bot_game;
  };
}
