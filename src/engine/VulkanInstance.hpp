#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP

#include <vulkan/vulkan.h>

#include <string>

#include "ValidationLayer.hpp"

namespace engine {

class VulkanInstance {
 public:
  [[maybe_unused]] explicit VulkanInstance(const std::string &applicationName);

  virtual ~VulkanInstance();

  [[nodiscard]] VkInstance getHandle() const;

 private:
  VkInstance m_instance;

  VkInstance createInstance(const std::string &applicationName);
};

}  // namespace engine

#endif  // VULKAN_INSTANCE_HPP
