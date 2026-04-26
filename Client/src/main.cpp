#include <array>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>

// IXWebSocket
#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXUserAgent.h"

#include "window/tetris_window/tetris_window.hpp"
#include "window/super_tetris_window/super_tetris_window.hpp"
#include "render/render.hpp"
#include "tetris/tetris.hpp"
#include "utils/timer.hpp"

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

int main() {
  tetris_bi::super_tetris_window tw;
  tw.run();
}
