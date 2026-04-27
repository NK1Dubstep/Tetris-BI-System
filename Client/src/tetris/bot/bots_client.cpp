/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "tetris/tetris.hpp"
#include "bots_client.hpp"

#include "nlohmann/json.hpp"

#include <iostream>

namespace tetris_bi {

  namespace {
    auto generate_random_idle_exit() {
      return generate_randui32() % 10;
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
    connect();
    for (int i = 0; i < bots.size(); i++) {
      bot_register(i);
    }
  }

  void bots_client::update() {
    std::lock_guard guard(bots_mutex);

    tim.update();
    auto dt = tim.delta_time_p;

    for (auto &bot : bots) {
      auto stats = bot.game.pop_last_played_session_stats();
      if (is_connected && bot.id.has_value() && stats.has_value()) {
        nlohmann::json js = nlohmann::json{
          {"type", "send_stats"},
          {"session", {{"id", bot.id.value()}, {"figure_passed", stats->prog.figure_passed}}}
        };
        ws.send(js.dump());
      }

      tetris_game::state st;
      bot.game.update(dt);
      st = bot.game.get_state();
      if (st == tetris_game::state::IDLE) {
        if (bot.prev_state == tetris_game::state::SESSION) {
          set_is_playing_tetris(bot, false);
          bot.idle_exit = generate_random_idle_exit();
          bot.in_idle = 0;
        } else {
          bot.in_idle += dt;
        }
        if (bot.in_idle > bot.idle_exit) {
          bot.game.start_session();
        }
        bot.prev_state = tetris_game::state::IDLE;
      } else if (st == tetris_game::state::SESSION) {
        if (bot.prev_state == tetris_game::state::IDLE) {
          set_is_playing_tetris(bot, true);
          bot.session_exit = generate_random_session_exit();
          bot.in_session = 0;
          bot.last_tick = 0;
        } else {
          bot.in_session += dt;
          bot.last_tick += dt;
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

  void bots_client::connect() {
    ws.setUrl("ws://127.0.0.1:5837/ws");

    ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
      if (msg->type == ix::WebSocketMessageType::Open) {
        std::cout << "WebSocket connected!" << std::endl;
        is_connected = true;
      }
      else if (msg->type == ix::WebSocketMessageType::Close) {
        std::cout << "WebSocket closed" << std::endl;
        is_connected = false;
      }
      else if (msg->type == ix::WebSocketMessageType::Message) {
        nlohmann::json js = nlohmann::json::parse(msg->str);

        auto id = js["id"];
        auto bot_index = js["bot_index"];

        std::lock_guard guard(bots_mutex);
        bots[bot_index].id = id;
      }
      else if (msg->type == ix::WebSocketMessageType::Error) {
        std::cerr << "WebSocket error: " << msg->errorInfo.reason << std::endl;
      }
    });

    ws.start();

    for (int i = 0; i < 50 && !is_connected; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!is_connected) {
      std::cout << "Bots client: websocket is not connected in 5 seconds. Fail\n";
      ws.stop();
    }
  }

  void bots_client::bot_register(int i) {
    if (is_connected) {
      ws.send(R"({"type":"register", "bot_index": )" + std::to_string(i) + "}");
    }
  }

  void bots_client::set_is_playing_tetris(bot &bt, bool b) {
    if (is_connected && bt.id.has_value()) {
      ws.send(R"({"type":"set_is_playing_tetris", "id": )" + std::to_string(bt.id.value()) +
        R"(, "value": )" + std::to_string(static_cast<int>(b)) + "}");
    }
  }
}
