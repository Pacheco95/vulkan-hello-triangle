#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class FrameBuffer {
 public:
  FrameBuffer(VkDevice device, const VkFramebufferCreateInfo& framebufferInfo,
              const VkAllocationCallbacks* alloc = nullptr)
      : m_device(device), m_alloc(alloc) {
    ABORT_ON_FAIL(
        vkCreateFramebuffer(device, &framebufferInfo, alloc, &m_frameBuffer),
        "failed to create framebuffer!");
    SPDLOG_DEBUG("Created frame buffer: {}", fmt::ptr(m_frameBuffer));
  }

  virtual ~FrameBuffer() {
    vkDestroyFramebuffer(m_device, m_frameBuffer, m_alloc);
    SPDLOG_DEBUG("Destroyed frame buffer: {}", fmt::ptr(m_frameBuffer));
  }

  operator VkFramebuffer() {return m_frameBuffer; }

 private:
  VkFramebuffer m_frameBuffer = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // FRAME_BUFFER_HPP
