#include <array>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>

#define MINIFB_IMPLEMENTATION
#include "MiniFB_cpp.h"

#include "render/render.hpp"

int main() {
  constexpr uint32_t W = 800;
  constexpr uint32_t H = 600;

  render rnd(W, H);

  std::vector<std::vector<uint32_t>> field(10, std::vector<uint32_t>(30));

  field[4][4] = 1;
  field[5][4] = 1;
  field[4][5] = 1;
  field[4][3] = 1;

  rnd.render_tetris(field);

  mfb_window *window = mfb_open_ex("Nk1 + av1 Super tertis", W, H, WF_RESIZABLE);
  if (!window) {
    return 1;
  }
  mfb_set_target_fps(1000);

  mfb_update_ex(window, rnd.get_buffer().data(), W, H);

  mfb_timer *timer = mfb_timer_create();
  mfb_timer_reset(timer);

  double delta_time = mfb_timer_delta(timer);
  double time_since_fps_update = 0;
  int frame_count = 0;

  do {
    delta_time = mfb_timer_delta(timer);
    time_since_fps_update += delta_time;
    frame_count++;

    mfb_update_ex(window, rnd.get_buffer().data(), W, H);

    if (time_since_fps_update > 1) {
      std::cout << frame_count << "FPS," << 1 / delta_time << "instantFPS\r";
      time_since_fps_update = 0;
      frame_count = 0;
    }
  } while(mfb_wait_sync(window));

  mfb_timer_destroy(timer);
  mfb_close(window);

  return 0;
}
