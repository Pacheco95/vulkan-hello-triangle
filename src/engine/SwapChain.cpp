#include "SwapChain.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "Abort.hpp"

uint32_t getImageCount(
    const engine::SwapChainSupportDetails &swapChainSupport) {
  bool hasLimitedImages = swapChainSupport.capabilities.maxImageCount > 0;
  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if (hasLimitedImages &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }
  return imageCount;
}

namespace engine {
SwapChain::SwapChain(GLFWwindow *window, VkPhysicalDevice physicalDevice,
                     VkDevice device, VkSurfaceKHR surface)
    : m_swapChain(nullptr), m_device(device) {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);

  m_swapChainImageFormat = surfaceFormat.format;

  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);

  m_swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities, window);

  uint32_t imageCount = getImageCount(swapChainSupport);

  SPDLOG_DEBUG("Using swap chain with {} images", imageCount);

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = m_swapChainExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  QueueFamilyIndices indices =
      QueueFamilyUtils::findQueueFamilies(physicalDevice, surface);

  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
    SPDLOG_DEBUG("Image sharing mode set to {}", "VK_SHARING_MODE_CONCURRENT");
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;      // Optional
    createInfo.pQueueFamilyIndices = nullptr;  // Optional
    SPDLOG_DEBUG("Image sharing mode set to {}", "VK_SHARING_MODE_EXCLUSIVE");
  }

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapChain) !=
      VK_SUCCESS) {
    ABORT("Failed to create swap chain");
  }

  vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, nullptr);
  m_swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount,
                          m_swapChainImages.data());
}

SwapChain::~SwapChain() {
  vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  SPDLOG_DEBUG("{} physical device surface formats found", formatCount);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  SPDLOG_DEBUG("{} present modes found", presentModeCount);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      SPDLOG_DEBUG("Ideal surface format found");
      return availableFormat;
    }
  }

  SPDLOG_DEBUG("Unable to find ideal surface format. Falling back to default");

  return availableFormats.front();
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      SPDLOG_DEBUG("Ideal present mode found: VK_PRESENT_MODE_MAILBOX_KHR");
      return availablePresentMode;
    }
  }

  SPDLOG_DEBUG(
      "Unable to find present mode VK_PRESENT_MODE_MAILBOX_KHR. Falling back "
      "to VK_PRESENT_MODE_FIFO_KHR");

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
  auto maxUint32 = std::numeric_limits<uint32_t>::max();

  bool didVulkanDetectedOptimalExtent =
      capabilities.currentExtent.width != maxUint32;

  if (didVulkanDetectedOptimalExtent) {
    SPDLOG_DEBUG("Ideal swap extent already set");
    return capabilities.currentExtent;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                             static_cast<uint32_t>(height)};

  uint32_t minExtW = capabilities.minImageExtent.width;
  uint32_t maxExtW = capabilities.maxImageExtent.width;
  actualExtent.width = std::clamp(actualExtent.width, minExtW, maxExtW);

  uint32_t minExtH = capabilities.minImageExtent.height;
  uint32_t maxExtH = capabilities.maxImageExtent.height;
  actualExtent.height = std::clamp(actualExtent.height, minExtH, maxExtH);

  return actualExtent;
}
}  // namespace engine
