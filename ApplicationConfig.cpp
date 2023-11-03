#include "ApplicationConfig.hpp"

const std::size_t ApplicationConfig::WINDOW_WIDTH =
    static_cast<std::size_t>(800);

const std::size_t ApplicationConfig::WINDOW_HEIGHT =
    static_cast<std::size_t>(600);

const char *ApplicationConfig::WINDOW_TITLE = "Hello Triangle";

#ifdef NDEBUG
const bool ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED = false;
#else
const bool ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED = true;
#endif

const std::vector<const char *> ApplicationConfig::VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation"};
