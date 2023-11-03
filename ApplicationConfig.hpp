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

const std::size_t ApplicationConfig::WINDOW_WIDTH = static_cast<std::size_t>(800);
const std::size_t ApplicationConfig::WINDOW_HEIGHT = static_cast<std::size_t>(600);
const char *ApplicationConfig::WINDOW_TITLE = "Hello Triangle";

#ifdef NDEBUG
const bool ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED = false;
#else
const bool ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED = true;
#endif

const std::vector<const char *> ApplicationConfig::VALIDATION_LAYERS{
        "VK_LAYER_KHRONOS_validation"
};

#endif //APPLICATION_CONFIG_HPP
