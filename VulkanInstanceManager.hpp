#ifndef VULKAN_INSTANCE_MANAGER_HPP
#define VULKAN_INSTANCE_MANAGER_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace engine {

class VulkanInstanceManager {
 public:
  [[maybe_unused]] explicit VulkanInstanceManager(std::string applicationName);

  virtual ~VulkanInstanceManager();

 private:
  VkInstance m_instance;
  std::string m_applicationName;
  VkDebugUtilsMessengerEXT m_debugMessenger;

  void setupDebugMessenger();
  void createInstance();
  static bool checkValidationLayerSupport();
};

}  // namespace engine

#endif  // VULKAN_INSTANCE_MANAGER_HPP
