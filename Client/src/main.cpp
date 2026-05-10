/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "window/tetris_window/tetris_window.hpp"
#include "window/super_tetris_window/super_tetris_window.hpp"
#include "window/super_puper_tetris_window/super_puper_tetris_window.hpp"

#include "utils/audio.hpp"

#define SUPER_PUPER_TETRIS_WINDOW
#define MUSIC

int main() {

#ifdef MUSIC
  tetris_bi::audio_player::get().play_loop("music/TetrisTheme.mp3");
#endif  // MUSIC

#ifdef TETRIS_WINDOW
  tetris_bi::tetris_window tw;
  tw.run();
#endif  // TETRIS_WINDOW

#ifdef SUPER_TETRIS_WINDOW
  tetris_bi::super_tetris_window stw;
  stw.run();
#endif  // SUPER_TETRIS_WINDOW

#ifdef SUPER_PUPER_TETRIS_WINDOW
  tetris_bi::super_puper_tetris_window sptw;
  sptw.run();
#endif  // SUPER_PUPER_TETRIS_WINDOW
}
