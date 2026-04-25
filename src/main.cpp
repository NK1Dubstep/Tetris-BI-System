#include <array>
#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>

#define MINIFB_IMPLEMENTATION
#include "MiniFB_cpp.h"

#include "window/tetris_window/tetris_window.hpp"
#include "window/super_tetris_window/super_tetris_window.hpp"
#include "render/render.hpp"
#include "tetris/tetris.hpp"
#include "utils/timer.hpp"
#include "tetris/bot/bot.hpp"

/* 
  
    В итоге я пришел к тому, что лучшая модель - это когда есть тетрис, он просто существует, но он сам не следит за временем.
    У него есть update от дельта тайм - что это будет за дельта тайм, вычислять будет НЕ ТЕТРИС, а какой-нибудь таймер
    (например, в окне, который обновляется тогда, когда захочет MiniFB).
    
    То есть ДОПУСТИМ мы захотели сделать 6 тетрисов.

    поток1: тетрис1 --> управляется окном
    поток2: тетрис2,3,4,5 ---> просто в цикле прибавляется дельта тайм.
    поток3: тетрис6 ---> while (true) {tetris6.update(0);} --
              просто тупой пример, который показывает, что мы можем разделить тетрисы как угодно по потокам 
              и в каждом потоке выбрать способ управления временем тетриса.

    Посоветовался с клодом и в итоге это лучше чем та ХРЕНЬ которую я рассказывал в метро.


    у окна будет два режима, режим управления и режим наблюдателя за ботом.
    Когда режим управления, у нас в окне работает таймер, который связан и с инпутом, и с обновлением тетриса (как сейчас написано в tetris_window).
    Понятно, что когда режим наблюдателя, бот там сам руководит таймером и соотвественно сам будет знать когда ему че нажимать. Как будет в примере ниже
      (тут sleep_for нужен чтобы не перегружать процессор этим потоком). А от окна в этом режиме требуется только вывести игру.

    Возможно, эти 2 режима будут 2 разных класса окна.
    У нас сейчас tetris_window - это режим управления.

    Возможно, режим наблюдателя будет принимать сразу массив тетрисов, и отрисовывать только какой-то один, и переключение по стрелочкам.

    Возможно, можно сделать несколько окон.
    */

int main() {
  tetris_bi::super_tetris_window tw;
  
  std::vector<tetris_bi::tetris_game> games(10);
  std::mutex games_mutex;
  std::atomic<bool> at_end_of_main = false;

  tw.link_games_mutex(games_mutex);
  tw.link_tetris_games(games);

  // tw.run();
  
  at_end_of_main = true;
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
