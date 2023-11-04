#include "LogicalDevice.hpp"

#include <stdexcept>

#include "ApplicationConfig.hpp"
#include "QueueFamilyUtils.hpp"

namespace engine {
LogicalDevice::LogicalDevice(VkPhysicalDevice physicalDevice)
    : m_device(nullptr), m_graphicsQueue(nullptr) {
  QueueFamilyIndices indices =
      QueueFamilyUtils::findQueueFamilies(physicalDevice);

  float queuePriority = 1.0f;
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;

  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    auto &validationLayers = ApplicationConfig::VALIDATION_LAYERS;
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0,
                   &m_graphicsQueue);
}

LogicalDevice::~LogicalDevice() { vkDestroyDevice(m_device, nullptr); }
}  // namespace engine
