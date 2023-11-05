#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"
#include "SwapChain.hpp"

namespace engine {

class RenderPass {
 public:
  explicit RenderPass(VkDevice device, const SwapChain& swapChain);
  virtual ~RenderPass();

  [[nodiscard]] VkRenderPass getHandle() const;

 private:
  VkDevice m_device;
  VkRenderPass m_renderPass;
};

}  // namespace engine

#endif  // RENDER_PASS_HPP
