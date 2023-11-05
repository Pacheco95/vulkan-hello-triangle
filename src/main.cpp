#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>

#include "Application.hpp"

void configureLogger();

int main() {
  configureLogger();

  Application app{};

  try {
    app.run();
  } catch (const std::exception &e) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void configureLogger() {
  auto level = static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL);
  spdlog::set_level(level);
}
