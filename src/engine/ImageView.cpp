#include "ImageView.hpp"

#include "Abort.hpp"

namespace engine {
ImageView::ImageView(VkDevice device, const VkImageViewCreateInfo &createInfo,
                     const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(
      vkCreateImageView(device, &createInfo, alloc, &m_swapChainImageViews),
      "Failed to create image view");

  SPDLOG_DEBUG("Created image view: {}", fmt::ptr(m_swapChainImageViews));
}

ImageView::~ImageView() {
  vkDestroyImageView(m_device, m_swapChainImageViews, m_alloc);
  SPDLOG_DEBUG("Destroyed image view: {}", fmt::ptr(m_swapChainImageViews));
}
}  // namespace engine
