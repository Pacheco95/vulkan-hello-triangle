#include "PhysicalDevice.hpp"

#include "Abort.hpp"

namespace engine {

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

}  // namespace engine
