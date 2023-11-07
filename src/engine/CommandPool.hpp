#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"
#include "QueueFamilyUtils.hpp"

namespace engine {

class CommandPool {
 public:
  explicit CommandPool(VkDevice device, uint32_t queueFamilyIndex)
      : m_device(device), commandPool(nullptr) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    ABORT_ON_FAIL(
        vkCreateCommandPool(m_device, &poolInfo, nullptr, &commandPool),
        "Failed to create command pool");
  }

  virtual ~CommandPool() {
    vkDestroyCommandPool(m_device, commandPool, nullptr);
  }

  [[nodiscard]] VkCommandPool getHandle() const { return commandPool; }

 private:
  VkCommandPool commandPool;
  VkDevice m_device;
};

}  // namespace engine

#endif  // COMMAND_POOL_HPP
