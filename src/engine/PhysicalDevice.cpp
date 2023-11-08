#include "PhysicalDevice.hpp"

#include "Abort.hpp"
#include "QueueFamily.hpp"

namespace engine {
VkPhysicalDevice PhysicalDevice::pick(VkInstance instance,
                                      VkSurfaceKHR surface) {
  std::vector<VkPhysicalDevice> devices = enumeratePhysicalDevices(instance);

  for (const auto& device : devices) {
    if (PhysicalDevice::isDeviceSuitable(device, surface)) {
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

std::vector<VkQueueFamilyProperties> PhysicalDevice::enumerateQueueFamilies(
    VkPhysicalDevice device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  return queueFamilies;
}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device,
                                      VkSurfaceKHR surface) {
  QueueFamilyIndices indices =
      QueueFamily::findSuitableQueueFamilies(device, surface);

  return indices.isComplete();
}
}  // namespace engine
