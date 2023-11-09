#include "QueueFamily.hpp"

#include "PhysicalDevice.hpp"

engine::QueueFamilyIndices engine::QueueFamily::findSuitableQueueFamilies(
    VkPhysicalDevice device, VkSurfaceKHR surface
) {
  QueueFamilyIndices indices;

  int i = 0;
  for (const auto& queueFamily :
       PhysicalDevice::enumerateQueueFamilies(device)) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

namespace engine {}  // namespace engine
