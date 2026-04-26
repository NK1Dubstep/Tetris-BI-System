/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "tetris/tetris.hpp"
#include "bots_client.hpp"

#include <iostream>

namespace tetris_bi {

  namespace {
    auto generate_random_idle_exit() {
      return generate_randui32() % 3 + 1;
    }

    auto generate_random_session_exit() {
      return generate_randui32() % 60 + 90;
    }
  }

  bots_client::bots_client(int n) :
    bots(n)
  {
    for (auto &bot : bots) {
      bot.idle_exit = generate_random_idle_exit();
      bot.session_exit = generate_random_session_exit();
    }
  }

  void bots_client::update(timer::seconds delta_time) {
    for (auto &bot : bots) {
      tetris_game::state st;
      bot.game.update(delta_time);
      st = bot.game.get_state();
      if (st == tetris_game::state::IDLE) {
        if (bot.prev_state == tetris_game::state::SESSION) {
          bot.idle_exit = generate_random_idle_exit();
          bot.in_idle = 0;
        } else {
          bot.in_idle += delta_time;
        }
        if (bot.in_idle > bot.idle_exit) {
          bot.game.start_session();
        }
        bot.prev_state = tetris_game::state::IDLE;
      } else if (st == tetris_game::state::SESSION) {
        if (bot.prev_state == tetris_game::state::IDLE) {
          bot.session_exit = generate_random_session_exit();
          bot.in_session = 0;
          bot.last_tick = 0;
        } else {
          bot.in_session += delta_time;
          bot.last_tick += delta_time;
        }
        if (bot.in_session > bot.session_exit) {
          bot.game.end_session();
        } else if (bot.last_tick > bot.TICK_INTERVAL) {
          uint32_t move_id = generate_randui32() % 5;
          switch (move_id) {
            case 0: bot.game.move_direction(1); break;
            case 1: bot.game.move_direction(-1); break;
            case 2: bot.game.rotateCW(); break;
            case 3: bot.game.rotateCCW(); break;
            default: break;
          }
          bot.last_tick = 0;
        }
        bot.prev_state = tetris_game::state::SESSION;
      }
    }
  }
}
