#include "QueueFamily.hpp"

#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.hpp"

engine::QueueFamilyIndices engine::QueueFamily::findIndices(
    const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface
) {
  QueueFamilyIndices indices;

  const auto &families = device.getQueueFamilyProperties();

  for (int familyIndex = 0; familyIndex < families.size(); familyIndex++) {
    const auto &props = families[familyIndex];

    if (props.queueFlags & vk::QueueFlagBits::eGraphics) {
      indices.graphicsFamily = familyIndex;
    }

    if (device.getSurfaceSupportKHR(familyIndex, surface)) {
      indices.presentFamily = familyIndex;
    }

    if (indices.isComplete()) {
      break;
    }
  }

  return indices;
}

namespace engine {}  // namespace engine
