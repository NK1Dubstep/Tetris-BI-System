/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#include "window/tetris_window/tetris_window.hpp"

#include "utils/audio.hpp"

// #define MUSIC  // uncomment if you want to listen background music

int main() {

#ifdef MUSIC
  tetris_bi::audio_player::get().play_loop("music/TetrisTheme.mp3");
#endif  // MUSIC

  tetris_bi::tetris_window tw;
  tw.run();
}
