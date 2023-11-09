#include "Semaphore.hpp"

namespace engine {
Semaphore::Semaphore(VkDevice device,
                     const VkSemaphoreCreateInfo &semaphoreInfo,
                     const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(vkCreateSemaphore(device, &semaphoreInfo, alloc, &m_semaphore),
                "Failed to create semaphore");

  SPDLOG_DEBUG("Created semaphore: {}", fmt::ptr(m_semaphore));
}

Semaphore::~Semaphore() {
  vkDestroySemaphore(m_device, m_semaphore, m_alloc);
  SPDLOG_DEBUG("Destroyed semaphore: {}", fmt::ptr(m_semaphore));
}

VkSemaphore Semaphore::getHandle() const { return m_semaphore; }
}  // namespace engine
