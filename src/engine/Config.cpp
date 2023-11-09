#include "Config.hpp"

namespace engine {
#ifdef NDEBUG
const bool Config::IS_VALIDATION_LAYERS_ENABLED = false;
#else
const bool Config::IS_VALIDATION_LAYERS_ENABLED = true;
#endif

}  // namespace engine
