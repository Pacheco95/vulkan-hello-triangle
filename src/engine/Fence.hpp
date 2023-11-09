#ifndef FENCE_HPP
#define FENCE_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class Fence {
 public:
  Fence(VkDevice device, const VkFenceCreateInfo& fenceInfo,
        const VkAllocationCallbacks* alloc = nullptr);

  virtual ~Fence();

  operator VkFence() { return m_fence; }

 private:
  VkFence m_fence = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // FENCE_HPP
