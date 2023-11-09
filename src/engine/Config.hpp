#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

namespace engine {
struct Config {
  static constexpr std::size_t WINDOW_WIDTH = 800;

  static constexpr std::size_t WINDOW_HEIGHT = 600;

  static constexpr auto APP_NAME = "Hello Triangle";

  static constexpr auto WINDOW_TITLE = "Hello Triangle";

  static constexpr std::array<const char *, 1> VALIDATION_LAYERS = {
      "VK_LAYER_KHRONOS_validation"};

  static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  static const bool IS_VALIDATION_LAYERS_ENABLED;

  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
}  // namespace engine

#endif  // CONFIG_HPP
