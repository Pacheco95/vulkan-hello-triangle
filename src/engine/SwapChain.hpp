#ifndef SWAP_CHAIN_HPP
#define SWAP_CHAIN_HPP

#include <GLFW/glfw3.h>

#include <vector>

#include "QueueFamilyUtils.hpp"

namespace engine {
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
 public:
  SwapChain(GLFWwindow *window, VkPhysicalDevice physicalDevice,
            VkDevice device, VkSurfaceKHR surface);

  virtual ~SwapChain();

  static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);

  [[nodiscard]] VkFormat getSwapChainImageFormat() const;

  [[nodiscard]] VkExtent2D getSwapChainExtent() const;

  const std::vector<VkImageView> &getImageViews();

  VkSwapchainKHR getHandle() const;

 private:
  VkSwapchainKHR m_swapChain;
  VkDevice m_device;
  std::vector<VkImage> m_swapChainImages;
  std::vector<VkImageView> m_swapChainImageViews;
  VkFormat m_swapChainImageFormat;
  VkExtent2D m_swapChainExtent{};

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  static VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  static VkExtent2D chooseSwapExtent(
      const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

  void createImageViews(VkDevice device);
};

}  // namespace engine

#endif  // SWAP_CHAIN_HPP
