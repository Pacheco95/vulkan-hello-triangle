#ifndef QUEUE_FAMILY_UTILS_HPP
#define QUEUE_FAMILY_UTILS_HPP

#include <vulkan/vulkan.h>

#include <optional>

namespace engine {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class QueueFamilyUtils {
 public:
  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);
};

}  // namespace engine

#endif  // QUEUE_FAMILY_UTILS_HPP
