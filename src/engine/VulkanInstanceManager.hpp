#ifndef VULKAN_INSTANCE_MANAGER_HPP
#define VULKAN_INSTANCE_MANAGER_HPP

#include <vulkan/vulkan.h>

#include <string>

#include "ValidationLayerManager.hpp"

namespace engine {

class VulkanInstanceManager {
 public:
  [[maybe_unused]] explicit VulkanInstanceManager(
      const std::string &applicationName);

  virtual ~VulkanInstanceManager();

  [[nodiscard]] VkInstance getHandle() const;

 private:
  VkInstance m_instance;

  VkInstance createInstance(const std::string &applicationName);
};

}  // namespace engine

#endif  // VULKAN_INSTANCE_MANAGER_HPP
