#include "FrameBuffer.hpp"

#include <stdexcept>

#include "Abort.hpp"

namespace engine {
FrameBuffer::FrameBuffer(VkDevice device, VkRenderPass renderPass,
                         const VkExtent2D &swapChainExtent,
                         VkImageView *attachments)
    : m_device(device) {
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments;
  framebufferInfo.width = swapChainExtent.width;
  framebufferInfo.height = swapChainExtent.height;
  framebufferInfo.layers = 1;

  ABORT_ON_FAIL(
      vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_frameBuffer),
      "Failed to create frame buffer");
}

FrameBuffer::~FrameBuffer() {
  vkDestroyFramebuffer(m_device, m_frameBuffer, nullptr);
}

std::vector<FrameBuffer> FrameBuffer::getFromImageViews(
    const std::vector<VkImageView> &swapChainImageViews,
    VkRenderPass renderPass, VkExtent2D swapChainExtent, VkDevice device) {
  std::vector<FrameBuffer> buffers;
  buffers.reserve(swapChainImageViews.size());

  for (auto &swapChainImageView : swapChainImageViews) {
    VkImageView attachments[] = {swapChainImageView};
    buffers.emplace_back(device, renderPass, swapChainExtent, attachments);
  }

  return buffers;
}
}  // namespace engine
