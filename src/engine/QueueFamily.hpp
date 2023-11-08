#ifndef QUEUE_FAMILY_HPP
#define QUEUE_FAMILY_HPP

#include <vulkan/vulkan.h>

#include <optional>

namespace engine {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value(); }
};

class QueueFamily {
 public:
  static QueueFamilyIndices findSuitableQueueFamilies(VkPhysicalDevice device);
};
}  // namespace engine

#endif  // QUEUE_FAMILY_HPP
