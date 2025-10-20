//
// Created by nikolina on 20.10.25..
//

#include "../include/MyApp.hpp"
#include <../../engine/libs/spdlog/include/spdlog/spdlog.h>

namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup completed");
}
} // app