#include "RenderPass.hpp"

namespace engine {
RenderPass::RenderPass(VkDevice device,
                       const VkRenderPassCreateInfo &renderPassInfo,
                       const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(
      vkCreateRenderPass(device, &renderPassInfo, alloc, &m_renderPass),
      "Failed to create render pass");
  SPDLOG_DEBUG("Created render pass: {}", fmt::ptr(m_renderPass));
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(m_device, m_renderPass, m_alloc);
  SPDLOG_DEBUG("Destroyed render pass: {}", fmt::ptr(m_renderPass));
}

VkRenderPass RenderPass::getHandle() const { return m_renderPass; }
}  // namespace engine
