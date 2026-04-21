/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "tetris/tetris.hpp"
#include "bot.hpp"

namespace tetris_bi {
  bot::bot(tetris_game& game_) : bot_game(game_) {
    uint32_t id_type = generate_randui32() % 2;

    switch (id_type) {
    case 0:
      // GET all stats from db
      break;
    case 1:
      m_bot_id = generate_randui32();
      break;
    }
  }

  void bot::make_move() {
    uint32_t move_id = generate_randui32() % 5;

    switch (move_id) {
    case 0:
      bot_game.move_direction(1);
      break;
    case 1:
      bot_game.move_direction(-1);
      break;
    case 2:
      bot_game.rotateCW();
      break;
    case 3:
      bot_game.rotateCCW();
      break;
    default:
      break;
    }
  }

  bool bot::send_stats() {
    // send all stats to db
    return true;
  }
}
