#include "PhysicalDeviceSelector.hpp"

#include <stdexcept>
#include <vector>

namespace engine {
PhysicalDeviceSelector::PhysicalDeviceSelector(VkInstance vkInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    if (isDeviceSuitable(device)) {
      m_selectedDevice = device;
      break;
    }
  }

  if (m_selectedDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool PhysicalDeviceSelector::isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);

  return indices.isComplete();
}

PhysicalDeviceSelector::QueueFamilyIndices
PhysicalDeviceSelector::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  for (int i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }
  }

  return indices;
}

VkPhysicalDevice PhysicalDeviceSelector::getSelectedDevice() const {
  return m_selectedDevice;
}
}  // namespace engine
