#ifndef LOGICAL_DEVICE_HPP
#define LOGICAL_DEVICE_HPP

#include <vulkan/vulkan.h>

#include <vector>

#include "QueueFamilyUtils.hpp"

namespace engine {

class LogicalDevice {
 public:
  explicit LogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

  virtual ~LogicalDevice();

  [[nodiscard]] VkQueue getGraphicsQueue() const { return m_graphicsQueue; };

  [[nodiscard]] VkDevice getDevice() const;

 private:
  VkDevice m_device;
  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;

  static std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos(
      engine::QueueFamilyIndices &indices);
};

}  // namespace engine

#endif  // LOGICAL_DEVICE_HPP
