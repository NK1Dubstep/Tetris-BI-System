/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#pragma once

#include <chrono>
#include <optional>

namespace tetris_bi {
  namespace ch = std::chrono;

  struct timer {
    using clock_t = ch::high_resolution_clock;
    using time_point = ch::time_point<clock_t>;
    using seconds = double;

  private:
    time_point
      tp_timer_start,
      tp_frame,
      tp_frame_p,
      tp_fps_measure_last;
    std::optional<time_point> tp_pause_start{std::nullopt};
    uint32_t frame_count{0}, frame_count_p{0};
    seconds fps_update_time{1.0};

  public:
    bool is_pause{false};
    seconds
      time{0.0},
      delta_time{0.0},
      time_p{0.0},
      delta_time_p{0.0},
      pause_total{0.0},
      pause_from_last{0.0};
    double fps{0}, fps_p{0};

    timer() {
      time_point now = clock_t::now();
      tp_timer_start = now;
      tp_frame = now;
      tp_frame_p = now;
      tp_fps_measure_last = now;
    }

    void update() {
      time_point tp_now = clock_t::now();

      delta_time = ch::duration_cast<ch::nanoseconds>(tp_now - tp_frame).count() / 1e9;
      delta_time_p = ch::duration_cast<ch::nanoseconds>(tp_now - tp_frame_p).count() / 1e9;

      auto fps_mt = ch::duration_cast<ch::nanoseconds>(tp_now - tp_fps_measure_last).count() / 1e9;

      if (fps_mt >= fps_update_time) {
        fps = frame_count / fps_mt;
        fps_p = frame_count_p / fps_mt;
        frame_count = frame_count_p = 0;
        tp_fps_measure_last = tp_now;
      }

      if (is_pause) {
        if (tp_pause_start == std::nullopt) {
          tp_pause_start = tp_now;
        }
        pause_total += delta_time;
        pause_from_last += delta_time;
        time += delta_time;
        tp_frame = tp_now;
        frame_count++;
        return;
      }

      if (tp_pause_start != std::nullopt) {
        tp_pause_start = std::nullopt;
        pause_from_last = 0;
      }

      time += delta_time;
      time_p += delta_time;

      tp_frame_p = tp_now;
      tp_frame = tp_now;
      frame_count++;
      frame_count_p++;
    }

    void pause_set(bool pause) {
      is_pause = pause;
    }

    void fps_update_time_set(seconds sec) {
      fps_update_time = sec;
    }

    void pause_switch() {
      is_pause = !is_pause;
    }

    void reset() {
      *this = timer();
    }
  };}