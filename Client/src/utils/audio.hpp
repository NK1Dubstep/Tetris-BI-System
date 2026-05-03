/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include "miniaudio.h"
#include <string>

namespace tetris_bi {
  class audio_player {
  private:
    ma_engine engine;
    bool is_ok{true};

    audio_player() {
      ma_result result = ma_engine_init(NULL, &engine);
      if (result != MA_SUCCESS) {
        is_ok =  false;
      }
    }

    ~audio_player() {
      if (is_ok) {
        ma_engine_uninit(&engine);
      }
    }

    audio_player(const audio_player &) = delete;
    audio_player &operator=(const audio_player &) = delete;

  public:

    static audio_player &get() {
      static audio_player instance;
      return instance;
    }

    void play(const std::string &path) {
      if (is_ok) {
        ma_engine_play_sound(&engine, path.c_str(), NULL);
      }
    }
  };
}
