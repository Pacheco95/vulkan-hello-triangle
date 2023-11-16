#ifndef VALIDATION_LAYER_HPP
#define VALIDATION_LAYER_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

namespace engine {

class ValidationLayer {
 public:
  explicit ValidationLayer(vk::UniqueInstance& instance);

  virtual ~ValidationLayer();

  static bool checkLayers(
      std::vector<char const*> const& layers,
      std::vector<vk::LayerProperties> const& properties
  );

 private:
  vk::UniqueInstance& m_instance;
  vk::DebugUtilsMessengerEXT debugUtilsMessenger;
  static uint32_t s_errorsCount;

 private:
  void setupDebugMessenger();

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
      void* /*pUserData*/
  );
};

}  // namespace engine

#endif  // VALIDATION_LAYER_HPP
