/* ==========[ TETRIS BI SYSTEM ]==========
 * Copyright (c) 2026 Tetris-BI-System Contributors
 * SPDX-License-Identifier: MIT
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : Ilya Pshenichniy
 * ======================================== */

#pragma once

#include "miniaudio.h"
#include <string>
#include <vector>

namespace tetris_bi {
  class audio_player {
  private:
    ma_engine engine;
    bool is_ok{true};
    std::vector<ma_sound *> loops;

    audio_player() {
      ma_result result = ma_engine_init(NULL, &engine);
      if (result != MA_SUCCESS) {
        is_ok =  false;
      }
    }

    void stop_loops() {
      for (auto *s : loops) {
        ma_sound_stop(s);
        ma_sound_uninit(s);
        delete s;
      }
      loops.clear();
    }

    ~audio_player() {
      if (is_ok) {
        stop_loops();
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

    void play_loop(const std::string &path) {
      if (!is_ok) return;

      ma_sound *sound = new ma_sound;
      if (ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, sound) != MA_SUCCESS) {
        delete sound;
        return;
      }
      ma_sound_set_looping(sound, MA_TRUE);
      ma_sound_start(sound);
      loops.push_back(sound);
    }
  };
}
