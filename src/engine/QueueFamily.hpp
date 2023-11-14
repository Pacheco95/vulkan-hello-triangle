#ifndef QUEUE_FAMILY_HPP
#define QUEUE_FAMILY_HPP

#include <optional>
#include <vulkan/vulkan.hpp>

namespace engine {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class QueueFamily {
 public:
  static engine::QueueFamilyIndices findIndices(
      const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface
  );
};
}  // namespace engine

#endif  // QUEUE_FAMILY_HPP
