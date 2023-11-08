#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace engine {

class PhysicalDevice {
 public:
  static VkPhysicalDevice pick(VkInstance instance, VkSurfaceKHR surface);

  static std::vector<VkPhysicalDevice> enumeratePhysicalDevices(
      VkInstance instance);

  static std::vector<VkQueueFamilyProperties> enumerateQueueFamilies(
      VkPhysicalDevice device);

  static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_HPP
