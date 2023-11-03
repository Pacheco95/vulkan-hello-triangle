#ifndef VULKAN_INSTANCE_MANAGER_HPP
#define VULKAN_INSTANCE_MANAGER_HPP

#include <vulkan/vulkan.h>

#include <string>

#include "ValidationLayerManager.hpp"

namespace engine {

class VulkanInstanceManager {
 public:
  [[maybe_unused]] explicit VulkanInstanceManager(std::string applicationName);

  virtual ~VulkanInstanceManager();

  VkInstance getInstance();

 private:
  VkInstance m_instance;
  std::string m_applicationName;

  VkInstance createInstance();
};

}  // namespace engine

#endif  // VULKAN_INSTANCE_MANAGER_HPP
