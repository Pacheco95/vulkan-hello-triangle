#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class RenderPass {
 public:
  RenderPass(VkDevice device, const VkRenderPassCreateInfo& renderPassInfo,
             const VkAllocationCallbacks* alloc);

  virtual ~RenderPass();

 private:
  VkRenderPass m_renderPass = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // RENDER_PASS_HPP
