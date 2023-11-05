#ifndef PHYSICAL_DEVICE_SELECTOR_HPP
#define PHYSICAL_DEVICE_SELECTOR_HPP

#include <vulkan/vulkan.h>

namespace engine {

class PhysicalDeviceSelector {
 public:
  explicit PhysicalDeviceSelector(VkInstance vkInstance, VkSurfaceKHR surface);

  [[nodiscard]] VkPhysicalDevice getHandle() const;

 private:
  VkPhysicalDevice m_selectedDevice = VK_NULL_HANDLE;

  static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

  static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_SELECTOR_HPP
