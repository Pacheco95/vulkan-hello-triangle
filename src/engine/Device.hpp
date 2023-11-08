#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <vulkan/vulkan.h>

namespace engine {

class Device {
 public:
  Device(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& createInfo,
         const VkAllocationCallbacks* allocator = nullptr);

  virtual ~Device();

  [[nodiscard]] VkDevice getHandler() const;

  [[nodiscard]] VkQueue getQueue(uint32_t familyIndex,
                                 uint32_t queueIndex = 0) const;

 private:
  VkDevice m_device = VK_NULL_HANDLE;
};

}  // namespace engine

#endif  // DEVICE_HPP