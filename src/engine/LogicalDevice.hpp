#ifndef LOGICAL_DEVICE_HPP
#define LOGICAL_DEVICE_HPP

#include <vulkan/vulkan.h>

namespace engine {

class LogicalDevice {
 public:
  explicit LogicalDevice(VkPhysicalDevice physicalDevice);

  virtual ~LogicalDevice();

 private:
  VkDevice m_device;
  VkQueue m_graphicsQueue;

 public:
  [[nodiscard]] VkQueue getGraphicsQueue() const { return m_graphicsQueue; };
};

}  // namespace engine

#endif  // LOGICAL_DEVICE_HPP
