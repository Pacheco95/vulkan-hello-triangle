#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>

#include <optional>

#include "Abort.hpp"

namespace engine {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value(); }
};

class PhysicalDevice {
 public:
  static VkPhysicalDevice pick(VkInstance instance);

  static std::vector<VkPhysicalDevice> enumeratePhysicalDevices(
      VkInstance instance);

  static QueueFamilyIndices findSuitableQueueFamilies(VkPhysicalDevice device);

  static std::vector<VkQueueFamilyProperties> enumerateQueueFamilies(
      VkPhysicalDevice device);

  static bool isDeviceSuitable(VkPhysicalDevice device);
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_HPP
