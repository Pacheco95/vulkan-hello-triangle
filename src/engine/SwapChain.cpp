#include "SwapChain.hpp"

namespace engine {
SwapChain::SwapChain(VkDevice device,
                     const VkSwapchainCreateInfoKHR &createInfo,
                     const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(vkCreateSwapchainKHR(device, &createInfo, alloc, &m_swapChain),
                "Failed to create swap chain");
}

SwapChain::~SwapChain() {
  vkDestroySwapchainKHR(m_device, m_swapChain, m_alloc);
}

}  // namespace engine
