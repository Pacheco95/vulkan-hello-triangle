#include "PhysicalDevice.hpp"

namespace engine {
VkPhysicalDevice PhysicalDevice::pick(VkInstance instance) {
  std::vector<VkPhysicalDevice> devices = enumeratePhysicalDevices(instance);

  for (const auto& device : devices) {
    if (PhysicalDevice::isDeviceSuitable(device)) {
      return device;
    }
  }

  ABORT("Failed to find a suitable GPU");
}

std::vector<VkPhysicalDevice> PhysicalDevice::enumeratePhysicalDevices(
    VkInstance instance) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    ABORT("Failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
  return devices;
}

QueueFamilyIndices PhysicalDevice::findSuitableQueueFamilies(
    VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  int i = 0;
  for (const auto& queueFamily : enumerateQueueFamilies(device)) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::enumerateQueueFamilies(
    VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices =
      PhysicalDevice::findSuitableQueueFamilies(device);

  return indices.isComplete();
}
}  // namespace engine
