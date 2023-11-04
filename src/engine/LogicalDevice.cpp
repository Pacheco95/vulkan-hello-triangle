#include "LogicalDevice.hpp"

#include <set>
#include <stdexcept>

#include "ApplicationConfig.hpp"
#include "QueueFamilyUtils.hpp"

namespace engine {
LogicalDevice::LogicalDevice(VkPhysicalDevice physicalDevice,
                             VkSurfaceKHR surface)
    : m_device(nullptr), m_graphicsQueue(nullptr), m_presentQueue(nullptr) {
  QueueFamilyIndices indices =
      QueueFamilyUtils::findQueueFamilies(physicalDevice, surface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos =
      getQueueCreateInfos(indices);

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = queueCreateInfos.size();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;

  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    auto &validationLayers = ApplicationConfig::VALIDATION_LAYERS;
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }

  vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0,
                   &m_graphicsQueue);

  vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

std::vector<VkDeviceQueueCreateInfo> LogicalDevice::getQueueCreateInfos(
    QueueFamilyIndices &indices) {
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  float queuePriority = 1.0f;

  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  return queueCreateInfos;
}

LogicalDevice::~LogicalDevice() { vkDestroyDevice(m_device, nullptr); }
}  // namespace engine
