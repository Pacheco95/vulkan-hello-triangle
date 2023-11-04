#include "PhysicalDeviceSelector.hpp"

#include <stdexcept>
#include <vector>

#include "QueueFamilyUtils.hpp"

using engine::QueueFamilyIndices;
using engine::QueueFamilyUtils;

namespace engine {
PhysicalDeviceSelector::PhysicalDeviceSelector(VkInstance vkInstance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      m_selectedDevice = device;
      break;
    }
  }

  if (m_selectedDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU");
  }
}

bool PhysicalDeviceSelector::isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = QueueFamilyUtils::findQueueFamilies(device);
  return indices.isComplete();
}

VkPhysicalDevice PhysicalDeviceSelector::getSelectedDevice() const {
  return m_selectedDevice;
}
}  // namespace engine
