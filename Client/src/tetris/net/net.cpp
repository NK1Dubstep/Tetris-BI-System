/* Tetris BI System
 * Authors : -= Vysotin Artem =-, -= Nikita Koptelov =-
 * Academic Director : IlyaP
 */

#include "net.hpp"

namespace tetris_bi {

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

  httplib::Client net::httpclient("127.0.0.1:5837");
}