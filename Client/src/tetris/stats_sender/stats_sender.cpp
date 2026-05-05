/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "stats_sender.hpp"

namespace tetris_bi {

  const std::string stats_sender::HTTPCLIENT_URL = "127.0.0.1:5837";
  const std::string stats_sender::WEBSOCKET_URL = "ws://127.0.0.1:5837/ws";

  struct ws_dummy_struct {
    ws_dummy_struct() {
      ix::initNetSystem();
    }
    ~ws_dummy_struct() {
      ix::uninitNetSystem();
    }
  };
  namespace {
    ws_dummy_struct ws_initializer;
  }

  void stats_sender::send_json(const nlohmann::json &data) {
    if (is_connected) {
      std::lock_guard lock(send_mutex);
      ws.send(data.dump());
    }
  }

  void stats_sender::send_raw(const std::string &s) {
    if (is_connected) {
      std::lock_guard lock(send_mutex);
      ws.send(s);
    }
  }

  void stats_sender::register_batch(uint32_t number) {
    send_raw(R"({"type":"register_batch", "number": )" + std::to_string(number) + "}");
  }

  void stats_sender::connect(const timer::seconds timeout) {
    ws.setUrl(WEBSOCKET_URL);

    ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
      switch (msg->type) {

      case ix::WebSocketMessageType::Open: {
        std::cout << "Stats sender: WebSocket connected!" << std::endl;
        is_connected = true;
        on_open();
        break;
      }
      case ix::WebSocketMessageType::Close: {
        std::cout << "Stats sender: WebSocket closed" << std::endl;
        is_connected = false;
        on_close();
        break;
      }
      case ix::WebSocketMessageType::Message: {
        try {
          nlohmann::json data = nlohmann::json::parse(msg->str);
          on_message(data);
        } catch (const nlohmann::json::parse_error &e) {
          std::cerr << "Stats sender: JSON parse error: " << e.what() << '\n';
        }
        break;
      }

      case ix::WebSocketMessageType::Error: {
        std::cerr << "Stats sender: WebSocket error: " << msg->errorInfo.reason << std::endl;
        break;
      }
      default:
        break;

      }  // end of switch
    });

    ws.start();

    for (int i = 0; i < timeout * 10 && !is_connected; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!is_connected) {
      std::cout << std::format("Bots client: websocket is not connected in {} seconds. Fail\n", timeout);
      ws.stop();
    }
  }

  void stats_sender::disconnect() {
    ws.stop();
  }

  void stats_sender::set_is_playing_tetris_update
    (const uint32_t id, const bool value) {
    is_playing_tetris_updates[id] = value;
  }

  void stats_sender::increase_sessions() noexcept {
    played_sessions++;
  }

  void stats_sender::set_send_time(timer::seconds time) {
    send_time = time;
  }

  void stats_sender::flush_update(timer::seconds time) {
    if (is_connected && time - last_send_time > send_time) {
      nlohmann::json arr1 = nlohmann::json::array();
      auto &vec = arr1.get_ref<nlohmann::json::array_t&>();
      vec.reserve(is_playing_tetris_updates.size());

      for (auto &[id, value] : is_playing_tetris_updates) {
        arr1.push_back({{"id", id}, {"value", value}});
      }
      nlohmann::json arr2 = on_update_metrics();

      // set_is_playing_tetris_batch
      send_json(nlohmann::json{
        {"type", "set_is_playing_tetris_batch"},
        {"data", arr1}
        });
      is_playing_tetris_updates.clear();

      // increase_number_of_sessions
      send_json(nlohmann::json{
        {"type", "increase_number_of_sessions"},
        {"data", played_sessions}
        });
      played_sessions = 0;

      // update_metrics
      send_json(nlohmann::json{
        {"type", "update_metrics"},
        {"data", arr2}
        });

      last_send_time = time;
    }
  }
}
