#ifndef VALIDATION_LAYER_HPP
#define VALIDATION_LAYER_HPP

#include <vulkan/vulkan.h>

namespace engine {

class ValidationLayer {
 public:
  explicit ValidationLayer(VkInstance instance);

  virtual ~ValidationLayer();

  static void fillInstanceValidationLayerDebugInfo(
      VkInstanceCreateInfo &vkInstanceCreateInfo,
      const VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);

  static VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo();

 private:
  VkInstance m_vkInstance;
  VkDebugUtilsMessengerEXT m_debugMessenger;

  static VkResult createDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  static void destroyDebugUtilsMessengerEXT(
      VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger);

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT _messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      [[maybe_unused]] void *_pUserData);

  static bool checkValidationLayerSupport();
};

}  // namespace engine

#endif  // VALIDATION_LAYER_HPP
