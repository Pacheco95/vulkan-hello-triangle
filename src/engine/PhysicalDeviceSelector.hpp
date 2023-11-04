#ifndef PHYSICAL_DEVICE_SELECTOR_HPP
#define PHYSICAL_DEVICE_SELECTOR_HPP

#include <vulkan/vulkan.h>

namespace engine {

class PhysicalDeviceSelector {
 public:
  explicit PhysicalDeviceSelector(VkInstance vkInstance);

  [[nodiscard]] VkPhysicalDevice getSelectedDevice() const;

 private:
  VkPhysicalDevice m_selectedDevice = VK_NULL_HANDLE;

 private:
  static bool isDeviceSuitable(VkPhysicalDevice device);
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_SELECTOR_HPP