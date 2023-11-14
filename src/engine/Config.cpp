#include "Config.hpp"

namespace engine {
const std::vector<const char *> Config::VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool Config::IS_VALIDATION_LAYERS_ENABLED = false;
#else
const bool Config::IS_VALIDATION_LAYERS_ENABLED = true;
#endif

}  // namespace engine
