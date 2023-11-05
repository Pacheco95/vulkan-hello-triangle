#include "RenderPass.hpp"

namespace engine {
RenderPass::RenderPass(VkDevice device, const SwapChain& swapChain)
    : m_device(device), m_renderPass(nullptr) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapChain.getSwapChainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subPass{};
  subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subPass.colorAttachmentCount = 1;
  subPass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subPass;

  if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) !=
      VK_SUCCESS) {
    ABORT("failed to create render pass");
  }

  SPDLOG_DEBUG("Created render pass");
}

RenderPass::~RenderPass() {
  vkDestroyRenderPass(m_device, m_renderPass, nullptr);
}

VkRenderPass RenderPass::getPass() const { return m_renderPass; }
}  // namespace engine
