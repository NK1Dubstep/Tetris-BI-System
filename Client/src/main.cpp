/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "window/tetris_window/tetris_window.hpp"
#include "window/super_tetris_window/super_tetris_window.hpp"

#include "utils/audio.hpp"

int main() {
  tetris_bi::audio_player::get().play("music/TetrisTheme.mp3");

  tetris_bi::super_tetris_window stw;
  stw.run();

  // tetris_bi::tetris_game game;
  // tetris_bi::tetris_window tw;
  // tw.link_tetris(game);
  // tw.run();
}
