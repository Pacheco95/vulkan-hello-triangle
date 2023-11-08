#ifndef IMAGEVIEW_HPP
#define IMAGEVIEW_HPP

#include <vulkan/vulkan.h>

namespace engine {

class ImageView {
 public:
  ImageView(VkDevice device, const VkImageViewCreateInfo& createInfo,
            const VkAllocationCallbacks* alloc = nullptr);

  virtual ~ImageView();

  [[nodiscard]] VkImageView getHandle() const;

 private:
  VkImageView m_swapChainImageViews = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // IMAGEVIEW_HPP
