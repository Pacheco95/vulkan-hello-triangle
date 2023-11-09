#include "PipelineLayout.hpp"

namespace engine {
PipelineLayout::PipelineLayout(
    VkDevice device, const VkPipelineLayoutCreateInfo &pipelineLayoutInfo,
    const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(vkCreatePipelineLayout(device, &pipelineLayoutInfo, alloc,
                                       &m_pipelineLayout),
                "Failed to create pipeline layout");
  SPDLOG_DEBUG("Created pipeline layout: {}", fmt::ptr(m_pipelineLayout));
}

PipelineLayout::~PipelineLayout() {
  vkDestroyPipelineLayout(m_device, m_pipelineLayout, m_alloc);
  SPDLOG_DEBUG("Destroyed pipeline layout: {}", fmt::ptr(m_pipelineLayout));
}
}  // namespace engine
