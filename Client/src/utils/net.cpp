/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "net.hpp"

namespace tetris_bi {

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

  httplib::Client net::httpclient("127.0.0.1:5837");
}
