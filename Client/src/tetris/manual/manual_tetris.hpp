/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "utils/random.hpp"
#include "tetris/stats_sender/stats_sender.hpp"

#include <queue>
#include <unordered_map>
#include <memory>
#include <optional>

namespace tetris_bi {
  class manual_tetris : public stats_sender {
  public:
    manual_tetris();
    ~manual_tetris() { disconnect(); };
    
    void update();

    tetris_game &get_game() {return game;}

  private:
    timer tim;  // mega timer
    tetris_game game = tetris_game(0.5);

    std::atomic<std::optional<uint32_t>> id;
    tetris_game::state prev_state{tetris_game::state::IDLE};

    void ss_on_open() override;
    void ss_on_close() override;
    void ss_on_message(const nlohmann::json &data) override;
    nlohmann::json ss_on_update_metrics() override;
  };
}
