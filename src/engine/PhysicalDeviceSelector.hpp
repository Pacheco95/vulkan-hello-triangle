#ifndef PHYSICAL_DEVICE_SELECTOR_HPP
#define PHYSICAL_DEVICE_SELECTOR_HPP

#include <vulkan/vulkan.h>

#include <optional>

namespace engine {

class PhysicalDeviceSelector {
 public:
  explicit PhysicalDeviceSelector(VkInstance vkInstance);

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value(); }
  };

  [[nodiscard]] VkPhysicalDevice getSelectedDevice() const;

 private:
  VkPhysicalDevice m_selectedDevice = VK_NULL_HANDLE;

 private:
  static bool isDeviceSuitable(VkPhysicalDevice device);

  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

}  // namespace engine

#endif  // PHYSICAL_DEVICE_SELECTOR_HPP
