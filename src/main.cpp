#include <spdlog/spdlog.h>
#include <unistd.h>

#include <exception>

#include "Application.h"

void configureLogger();

int main() {
  SPDLOG_INFO("Process id {}", getpid());
  configureLogger();

  try {
    app::Application app{};
    app.run();
  } catch (const std::exception &e) {
    SPDLOG_CRITICAL(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void configureLogger() {
  auto level = static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL);
  spdlog::set_level(level);
}
