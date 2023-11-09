#ifndef IMAGEVIEW_HPP
#define IMAGEVIEW_HPP

#include <vulkan/vulkan.h>

namespace engine {

class ImageView {
 public:
  ImageView(VkDevice device, const VkImageViewCreateInfo& createInfo,
            const VkAllocationCallbacks* alloc = nullptr);

  virtual ~ImageView();

  operator VkImageView() {return m_swapChainImageViews; }
  operator VkImageView() const {return m_swapChainImageViews; }

 private:
  VkImageView m_swapChainImageViews = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // IMAGEVIEW_HPP
