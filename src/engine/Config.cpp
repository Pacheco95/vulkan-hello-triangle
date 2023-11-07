#include "Config.hpp"

#include <vulkan/vulkan.h>

namespace engine {
const std::size_t Config::WINDOW_WIDTH = static_cast<std::size_t>(800);

const std::size_t Config::WINDOW_HEIGHT = static_cast<std::size_t>(600);

const char *Config::APP_NAME = "Hello Triangle";
const char *Config::WINDOW_TITLE = "Hello Triangle";

#ifdef NDEBUG
const bool Config::IS_VALIDATION_LAYERS_ENABLED = false;
#else
const bool Config::IS_VALIDATION_LAYERS_ENABLED = true;
#endif

const std::vector<const char *> Config::VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> Config::DEVICE_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

}  // namespace engine
