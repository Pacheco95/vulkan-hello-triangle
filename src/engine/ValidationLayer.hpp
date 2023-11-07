#ifndef VALIDATION_LAYER_HPP
#define VALIDATION_LAYER_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace engine {

class ValidationLayer {
 public:
  explicit ValidationLayer(VkInstance instance);
  virtual ~ValidationLayer();
  static bool checkValidationLayerSupport();

  static void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);

 private:
  VkInstance m_instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
  void setupDebugMessenger();
};

}  // namespace engine

#endif  // VALIDATION_LAYER_HPP
