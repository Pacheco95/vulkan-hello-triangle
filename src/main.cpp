#include <spdlog/spdlog.h>
#include <unistd.h>

#include <exception>
#include <iostream>

#include "Application.hpp"

void configureLogger() {
  auto level = static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL);
  spdlog::set_level(level);
}

int main() {
  configureLogger();

  try {
    Application app{};
    SPDLOG_INFO("Process id {}", getpid());
    app.run();
  } catch (const std::exception &e) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
