/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "tetris/tetris.hpp"
#include "bot_client.hpp"

#include <iostream>

namespace tetris_bi {

  namespace {
    auto generate_random_idle_exit() {
      return generate_randui32() % 2 + 1;
    }

    auto generate_random_session_exit() {
      return generate_randui32() % 60 + 90;
    }
  }

  bot_client::bot_client() :
    idle_exit(generate_random_idle_exit()),
    session_exit(generate_random_session_exit())
  {
  }

  void bot_client::do_something(timer::seconds delta_time) {
    tetris_game::state st;
    game.update(delta_time);
    st = game.get_state();
    if (st == tetris_game::state::IDLE) {
      if (prev_state == tetris_game::state::SESSION) {
        idle_exit = generate_random_idle_exit();
        in_idle = 0;
      } else {
        in_idle += delta_time;
      }
      if (in_idle > idle_exit) {
        game.start_session();
      }
      prev_state = tetris_game::state::IDLE;
    } else if (st == tetris_game::state::SESSION) {
      if (prev_state == tetris_game::state::IDLE) {
        session_exit = generate_random_session_exit();
        in_session = 0;
        last_tick = 0;
      } else {
        in_session += delta_time;
        last_tick += delta_time;
      }
      if (in_session > session_exit) {
        game.end_session();
      } else if (last_tick > TICK_INTERVAL) {
        uint32_t move_id = generate_randui32() % 5;
        switch (move_id) {
          case 0: game.move_direction(1); break;
          case 1: game.move_direction(-1); break;
          case 2: game.rotateCW(); break;
          case 3: game.rotateCCW(); break;
          default: break;
        }
        last_tick = 0;
      }
      prev_state = tetris_game::state::SESSION;
    }
  }
}
