#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
#include <vector>

namespace engine {
struct Config {
  static const std::size_t WINDOW_WIDTH;
  static const std::size_t WINDOW_HEIGHT;
  static const char *APP_NAME;
  static const char *WINDOW_TITLE;
  static const std::vector<const char *> VALIDATION_LAYERS;
  static const std::vector<const char *> DEVICE_EXTENSIONS;
  static const bool IS_VALIDATION_LAYERS_ENABLED;
};
}  // namespace engine

#endif  // CONFIG_HPP
