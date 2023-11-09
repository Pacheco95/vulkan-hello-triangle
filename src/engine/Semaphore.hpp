#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class Semaphore {
 public:
  Semaphore(VkDevice device, const VkSemaphoreCreateInfo& semaphoreInfo,
            const VkAllocationCallbacks* alloc = nullptr);

  virtual ~Semaphore();

  operator VkSemaphore () {return m_semaphore; }

 private:
  VkSemaphore m_semaphore = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // SEMAPHORE_HPP
