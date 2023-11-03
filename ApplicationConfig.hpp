#ifndef APPLICATION_CONFIG_HPP
#define APPLICATION_CONFIG_HPP

#include <cstdint>
#include <vector>

struct ApplicationConfig {
  static const std::size_t WINDOW_WIDTH;
  static const std::size_t WINDOW_HEIGHT;
  static const char *WINDOW_TITLE;
  static const std::vector<const char *> VALIDATION_LAYERS;
  static const bool IS_VALIDATION_LAYERS_ENABLED;
};

#endif  // APPLICATION_CONFIG_HPP
