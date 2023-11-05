#include "PhysicalDeviceSelector.hpp"

#include <set>
#include <stdexcept>
#include <vector>

#include "Abort.hpp"
#include "ApplicationConfig.hpp"
#include "QueueFamilyUtils.hpp"
#include "SwapChain.hpp"

using engine::QueueFamilyIndices;
using engine::QueueFamilyUtils;

using std::runtime_error;
using std::set;
using std::string;
using std::vector;

namespace engine {
PhysicalDeviceSelector::PhysicalDeviceSelector(VkInstance vkInstance,
                                               VkSurfaceKHR surface) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    ABORT("Failed to find GPUs with Vulkan support");
  }

  vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSuitable(device, surface)) {
      m_selectedDevice = device;
      break;
    }
  }

  if (m_selectedDevice == VK_NULL_HANDLE) {
    ABORT("Failed to find a suitable GPU");
  }
}

bool PhysicalDeviceSelector::isDeviceSuitable(VkPhysicalDevice device,
                                              VkSurfaceKHR surface) {
  QueueFamilyIndices indices =
      QueueFamilyUtils::findQueueFamilies(device, surface);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool isSwapChainAdequate = false;

  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport =
        SwapChain::querySwapChainSupport(device, surface);
    isSwapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && isSwapChainAdequate;
}

VkPhysicalDevice PhysicalDeviceSelector::getSelectedDevice() const {
  return m_selectedDevice;
}

bool PhysicalDeviceSelector::checkDeviceExtensionSupport(
    VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  set<string> requiredExtensions(ApplicationConfig::DEVICE_EXTENSIONS.begin(),
                                 ApplicationConfig::DEVICE_EXTENSIONS.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}
}  // namespace engine
