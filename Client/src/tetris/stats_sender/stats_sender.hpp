/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

// IXWebSocket
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

// httplib
#include "httplib.h"

#include "nlohmann/json.hpp"

#include "utils/timer.hpp"

namespace tetris_bi {
  class stats_sender {
  private:
    static const std::string HTTPCLIENT_URL;
    static const std::string WEBSOCKET_URL;

    httplib::Client httpclient{HTTPCLIENT_URL};

    ix::WebSocket ws;
    std::atomic<bool> is_connected;

    std::mutex send_mutex;
    void send_json(const nlohmann::json &data);
    void send_raw(const std::string &data);

    std::unordered_map<uint32_t, bool> is_playing_tetris_updates;

    uint32_t played_sessions = 0;

    timer::seconds last_send_time{0};
    timer::seconds send_time{1};

  protected:
    virtual void ss_on_open() = 0;
    virtual void ss_on_close() = 0;
    virtual void ss_on_message(const nlohmann::json &data) = 0;
    virtual nlohmann::json ss_on_update_metrics() = 0;

    stats_sender() = default;

    void register_batch(uint32_t number);

    void connect(timer::seconds timeout);
    void disconnect();

    void set_is_playing_tetris_update(uint32_t id, bool value);
    void increase_sessions() noexcept;

    void set_send_time(timer::seconds time);

    void flush_update(timer::seconds time);

  public:
    bool get_connection_status() {return is_connected;}
  };
}
