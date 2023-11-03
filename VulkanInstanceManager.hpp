#ifndef VULKAN_INSTANCE_MANAGER_HPP
#define VULKAN_INSTANCE_MANAGER_HPP

#include <vulkan/vulkan.h>

#include <string>

namespace engine {

class VulkanInstanceManager {
 public:
  explicit VulkanInstanceManager(const std::string &applicationName);

  virtual ~VulkanInstanceManager();

  [[nodiscard]] bool checkValidationLayerSupport() const;

 private:
  VkInstance m_instance;
  std::string m_applicationName;
  VkDebugUtilsMessengerEXT m_debugMessenger;

  void setupDebugMessenger();

  void createInstance();
};

}  // namespace engine

#endif  // VULKAN_INSTANCE_MANAGER_HPP
