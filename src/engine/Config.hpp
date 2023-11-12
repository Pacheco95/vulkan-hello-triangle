#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

#ifdef NDEBUG
#define PREFIX "[Release]"
#else
#define PREFIX "[Debug]"
#endif

#define APPLICATION_NAME "Hello Triangle"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 1
#define APP_VERSION_PATCH 0


#define APP_VERSION_STRING                                       \
  "v" TOSTRING(APP_VERSION_MAJOR) "." TOSTRING(APP_VERSION_MINOR \
  ) "." TOSTRING(APP_VERSION_PATCH)


namespace engine {
struct Config {
  static constexpr std::size_t WINDOW_WIDTH = 800;

  static constexpr std::size_t WINDOW_HEIGHT = 600;

  static constexpr auto APP_VERSION = VK_MAKE_API_VERSION(
      0, APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH
  );

  static constexpr auto APP_NAME = APPLICATION_NAME;

  static constexpr auto WINDOW_TITLE =
      PREFIX " " APPLICATION_NAME " " APP_VERSION_STRING;

  static constexpr std::array<const char *, 1> VALIDATION_LAYERS{
      "VK_LAYER_KHRONOS_validation"};

  static constexpr std::array<const char *, 1> DEVICE_EXTENSIONS{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  static const bool IS_VALIDATION_LAYERS_ENABLED;

  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
}  // namespace engine

#endif  // CONFIG_HPP
