/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "bots_client.hpp"

namespace tetris_bi {

  int bots_client::bot_200iq::count_iso_cells(const tetris_game& game) {
    int iso_cells = 0;
    int count = 0;
    
    for (int x = 0; x < game.get_tetris_field().get_width(); x++) {
      count = 0;
      for (int y = game.get_tetris_field().get_height() - 1; y >= game.diff.lose_line; y--) {
        if (game.get_tetris_field().get_cell(x, y) != 0) {
          iso_cells += count;
          count = 0;
        } else {
          count++;
        }
      }
    }
    return iso_cells;
  }

  int bots_client::bot_200iq::eval(const tetris_game& game) {
    int score = 0;

    for (auto& p : game.get_current_figure().points) {
      score += p.y;
    }

    score -= (10 * count_iso_cells(game));

    return score;
  }

  void bots_client::bot_200iq::calculate_best_way() {
    tetris_game imaginary_game;
    imaginary_game = game;

    int best_score = -1e9;
    int best_r = 0;
    int best_m = 0;

    for (int curr_r = 0; curr_r < 4; curr_r++) {
      for (int curr_m = -5; curr_m <= 5; curr_m++) {
        for (int i = 0; i < curr_r; i++) imaginary_game.rotateCW();

        if (curr_m != 0) {
          int abs_curr_m = std::abs(curr_m);
          int dir = curr_m / abs_curr_m;
          for (int i = 0; i < abs_curr_m; i++) imaginary_game.move_direction(dir);
        }
        imaginary_game.drop();

        int new_score = eval(imaginary_game);
        if (new_score > best_score) {
          best_score = new_score;
          best_r = curr_r;
          best_m = curr_m;
        }
        imaginary_game = game;  // !
      }
    }

    best_rotate = best_r;
    best_move = best_m;
  }

  void bots_client::bot_200iq::make_move() {
    if (curr_figure_passed != game.get_prog().figure_passed) {
      curr_figure_passed = game.get_prog().figure_passed;
      best_move = std::nullopt, best_rotate = std::nullopt;
    }

    if (game.get_current_figure_high() <= 1) {
      return;
    }

    if (!(best_move.has_value() && best_rotate.has_value())) {
      calculate_best_way();
    }

    if (best_rotate.value() != 0) {
      game.rotateCW();
      (*best_rotate)--;
    } else if (best_move.value() != 0) {
      game.move_direction(best_move.value() / std::abs(best_move.value()));
      (*best_move) -= best_move.value() / std::abs(best_move.value());
    }
  }
}
