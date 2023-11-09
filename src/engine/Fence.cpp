#include "Fence.hpp"

namespace engine {
Fence::Fence(VkDevice device, const VkFenceCreateInfo &fenceInfo,
             const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(vkCreateFence(device, &fenceInfo, alloc, &m_fence),
                "Failed to create fence");

  SPDLOG_DEBUG("Created fence: {}", fmt::ptr(m_fence));
}

Fence::~Fence() {
  vkDestroyFence(m_device, m_fence, m_alloc);
  SPDLOG_DEBUG("Destroyed fence: {}", fmt::ptr(m_fence));
}

VkFence &Fence::getHandle() { return m_fence; }
}  // namespace engine
