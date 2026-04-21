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
  // /* EXAMPLE OF TETRIS WITH TIMER HANDLE FROM SELF-MADE CYCLE */
  // tetris_bi::tetris_game game0, game1;
  // tetris_bi::timer local_timer;
  // bool at_end_of_main = false;
  // 
  // 
  // /* EXAMPLE OF TETRIS WITH TIMER HANDLE FROM WINDOW */
  // tetris_bi::tetris_window tw;
  // tw.link_tetris(game1);
  // 
  // std::thread th([&](){
  //     while (!at_end_of_main) {
  //       local_timer.update();
  // 
  //       /* тут могут быть действия бота например */
  // 
  //       // if (local_timer.time > 10)
  //       //   tw.link_tetris(game0);  // lol так делать не надо потому что в тетрис виндоу уже есть таймер и апдейт тетриса, а этот цикл никуда не денется, тетрисом понятное дело должен руководить кто-то один, а тут мы 2 раза будем 2 дельта тайма прибавлять ахаххаха
  //       game0.update(local_timer.delta_time); std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
  //     }
  //   });
  // th.detach();  // Тетрисом game0 руководит цикл while (!at_end_of_main), дельта тайм вычисляется с помощью local_timer
  // 
  // tw.run(); // Окно руководит тетрисом game1, т.к. мы его прилинковали
  // 
  // at_end_of_main = true;
  // std::cout << "at end lol";

  bool at_end_of_main = false;
  
  tetris_bi::super_tetris_window tw;
  
  std::vector<tetris_bi::tetris_game> games(5);
  std::mutex games_mutex;
  
  auto bot_manager = [&](int a, int b) {
    tetris_bi::timer manager_timer;
    tetris_bi::timer::seconds BOTS_MOVE_INTERVAL = 0.15;
    tetris_bi::timer::seconds from_last_move = 0;

    std::vector<tetris_bi::bot> bots;
    
    for (int i = a; i <= b; i++) {
      bots.push_back(tetris_bi::bot(games[i]));
      std::cout << "created bot with id:" << bots[i - a].get_id() << "\n";
    }
  
    while (!at_end_of_main) {
      manager_timer.update();
      from_last_move += manager_timer.delta_time_p;
      {
        std::lock_guard lg(games_mutex);
        
        if (from_last_move > BOTS_MOVE_INTERVAL) {
          for (int i = a; i <= b; i++) {
            bots[i - a].make_move();
          }
          from_last_move = 0;
        }
  
        for (int i = a; i <= b; i++) {
          games[i].update(manager_timer.delta_time_p);
        }
      }
  
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  };
  
  tw.link_games_mutex(games_mutex);
  tw.link_tetris_games(games);
  
  std::thread thread1([&]() {bot_manager(0, 2); });
  thread1.detach();
  std::thread thread2([&]() {bot_manager(3, 4); });
  thread2.detach();
  
  tw.run();
  
  at_end_of_main = true;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
