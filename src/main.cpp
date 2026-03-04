#include <array>
#include <cstdint>
#include <iostream>

#define MINIFB_IMPLEMENTATION
#include "MiniFB_cpp.h"

int main() {
  constexpr uint32_t W = 800;
  constexpr uint32_t H = 600;

  mfb_window *window = mfb_open_ex("Nk1 + av1 Super tertis", W, H, WF_RESIZABLE);
  if (!window) {
    return 1;
  }
  mfb_set_target_fps(1000);

  static std::array<std::array<uint32_t, W>, H> buffer;

  auto put_pixel = [&](int x, int y, uint32_t c) {
      if (x < 0 || y < 0 || x >= W || y >= H) {
        return;
      }
      buffer[y][x] = c;
    };

  auto put_rect = [&](int lbx, int lby, int size, uint32_t c) {
      for (int y = lby; y < lby + size; y++) {
        for (int x = lbx; x < lbx + size; x++) {
          put_pixel(x, y, c);
        }
      }
    };

  mfb_update_state state;
  do {
    int x = rand() % W;
    int y = rand() % H;

    put_rect(x, y, 50, 0x834d18);
    state = mfb_update_ex(window, buffer.data(), W, H);
    put_rect(x, y, 50, 0);

    if (state != STATE_OK)
      break;

  } while(mfb_wait_sync(window));

  mfb_close(window);

  return 0;
}
