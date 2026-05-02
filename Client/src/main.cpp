#include <array>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>

#include "window/tetris_window/tetris_window.hpp"
#include "window/super_tetris_window/super_tetris_window.hpp"
#include "render/render.hpp"
#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "tetris/net/net.hpp"

int main() {
  // tetris_bi::super_tetris_window stw;
  // stw.run();

  tetris_bi::tetris_game game;
  tetris_bi::tetris_window tw;
  tw.link_tetris(game);
  tw.run();
}
