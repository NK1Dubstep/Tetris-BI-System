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
  // auto &x = tetris_bi::net::get();

  // auto id = tetris_bi::net::login();

  tetris_bi::super_tetris_window tw;
  tw.run();

  // tetris_bi::super_tetris_window tw;
  // tw.run();
}
