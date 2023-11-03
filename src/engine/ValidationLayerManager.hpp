#ifndef VALIDATION_LAYER_MANAGER_HPP
#define VALIDATION_LAYER_MANAGER_HPP

#include <vulkan/vulkan.h>

namespace engine {

class ValidationLayerManager {
 public:
  explicit ValidationLayerManager(VkInstance instance);

  virtual ~ValidationLayerManager();

  static void fillInstanceValidationLayerDebugInfo(
      VkInstanceCreateInfo &vkInstanceCreateInfo,
      const VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);

  static VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo();

 private:
  VkInstance m_instance;
  VkDebugUtilsMessengerEXT m_debugMessenger;

  static VkResult createDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  static void destroyDebugUtilsMessengerEXT(
      VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks *pAllocator);

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT _messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      [[maybe_unused]] void *_pUserData);

  static bool checkValidationLayerSupport();
};

}  // namespace engine

#endif  // VALIDATION_LAYER_MANAGER_HPP
