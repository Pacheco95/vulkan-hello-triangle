#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace engine {

class PhysicalDevice {
 public:
  static std::vector<VkQueueFamilyProperties> enumerateQueueFamilies(
      VkPhysicalDevice device
  );
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_HPP
