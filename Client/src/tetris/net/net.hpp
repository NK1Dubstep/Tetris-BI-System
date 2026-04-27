/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

// IXWebSocket
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

// httplib
#include "httplib.h"

#include "utils/random.hpp"

#include <nlohmann/json.hpp>

#include <iostream>

namespace tetris_bi {
  class net {
    static httplib::Client httpclient;

  public:
    static std::optional<uint32_t> login() {
      auto rnd = generate_randui32() % 2;

      auto as_new = []() -> std::optional<uint32_t> {
        auto res = httpclient.Post("/tetris_bi/login_as_new_player");
        if (res && res->status == 201) {
          auto json = nlohmann::json::parse(res->body);
          return json["id"];
        }
        return std::nullopt;
      };

      auto as_free = []() -> std::optional<uint32_t> {
        auto res = httpclient.Post("/tetris_bi/login_as_free_player");
        if (res && res->status == 200) {
          auto json = nlohmann::json::parse(res->body);
          return json["id"];
        }
        return std::nullopt;
      };

      if (rnd == 0) {
        return as_new();
      }
      else {
        auto res = as_free();
        if (res == std::nullopt) {
          return as_new();
        }
        return res;
      }
    }
  };
}
