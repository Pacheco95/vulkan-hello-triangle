#include "PhysicalDevice.hpp"

#include "Abort.hpp"

namespace engine {

std::vector<VkQueueFamilyProperties> PhysicalDevice::enumerateQueueFamilies(
    VkPhysicalDevice device
) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      device, &queueFamilyCount, queueFamilies.data()
  );
  return queueFamilies;
}

}  // namespace engine
