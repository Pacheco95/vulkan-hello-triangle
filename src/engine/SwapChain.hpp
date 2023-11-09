#ifndef SWAP_CHAIN_HPP
#define SWAP_CHAIN_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class SwapChain {
 public:
  SwapChain(VkDevice device, const VkSwapchainCreateInfoKHR& createInfo,
            const VkAllocationCallbacks* alloc = nullptr);

  virtual ~SwapChain();

  operator VkSwapchainKHR() { return m_swapChain; }

 private:
  VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // SWAP_CHAIN_HPP
