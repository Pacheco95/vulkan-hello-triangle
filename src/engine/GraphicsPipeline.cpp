#include "GraphicsPipeline.hpp"

namespace engine {
GraphicsPipeline::GraphicsPipeline(
    VkDevice device, const VkGraphicsPipelineCreateInfo &pipelineInfo,
    const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(
      vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, alloc,
                                &m_graphicsPipeline),
      "Failed to create graphics pipeline");
  SPDLOG_DEBUG("Created graphics pipeline: {}", fmt::ptr(m_graphicsPipeline));
}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyPipeline(m_device, m_graphicsPipeline, m_alloc);
  SPDLOG_DEBUG("Destroyed graphics pipeline: {}", fmt::ptr(m_graphicsPipeline));
}

VkPipeline GraphicsPipeline::getHandle() const { return m_graphicsPipeline; }
}  // namespace engine
