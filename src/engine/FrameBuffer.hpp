#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <vector>
#include <vulkan/vulkan.hpp>

namespace engine {

class FrameBuffer {
 public:
  explicit FrameBuffer(VkDevice device, VkRenderPass renderPass,
                       const VkExtent2D& swapChainExtent,
                       VkImageView* attachments);

  virtual ~FrameBuffer();

  static std::vector<FrameBuffer> getFromImageViews(
      const std::vector<VkImageView>& swapChainImageViews,
      VkRenderPass renderPass, VkExtent2D swapChainExtent, VkDevice device);

  VkFramebuffer m_frameBuffer = VK_NULL_HANDLE;
  VkDevice m_device;
};

}  // namespace engine

#endif  // FRAMEBUFFER_HPP
