#include "Device.hpp"

#include "Abort.hpp"

namespace engine {
Device::Device(VkPhysicalDevice physicalDevice,
               const VkDeviceCreateInfo &createInfo,
               const VkAllocationCallbacks *allocator)
    : m_alloc(allocator) {
  ABORT_ON_FAIL(
      vkCreateDevice(physicalDevice, &createInfo, allocator, &m_device),
      "Failed to create logical m_device");

  SPDLOG_DEBUG("Created Device: {}", fmt::ptr(m_device));
}

Device::~Device() {
  vkDestroyDevice(m_device, m_alloc);
  SPDLOG_DEBUG("Destroyed Device: {}", fmt::ptr(m_device));
}

VkQueue Device::getQueue(uint32_t familyIndex, uint32_t queueIndex) const {
  VkQueue queue;
  vkGetDeviceQueue(m_device, familyIndex, queueIndex, &queue);
  return queue;
}
}  // namespace engine
