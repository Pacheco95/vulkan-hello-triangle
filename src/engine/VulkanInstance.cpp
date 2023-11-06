#include "VulkanInstance.hpp"

#include <stdexcept>

#include "Abort.hpp"
#include "ApplicationConfig.hpp"
#include "Window.hpp"

namespace engine {
using engine::ValidationLayer;

[[maybe_unused]] VulkanInstance::VulkanInstance(
    const std::string &applicationName)
    : m_vkInstance(createInstance(applicationName)) {}

VkInstance VulkanInstance::createInstance(const std::string &applicationName) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = applicationName.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<const char *> requiredExtensions =
      Window::getRequiredExtensions();

  VkInstanceCreateInfo vkInstanceCreateInfo{};
  vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkInstanceCreateInfo.pApplicationInfo = &appInfo;
  vkInstanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
  vkInstanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
      ValidationLayer::getDebugMessengerCreateInfo();

  ValidationLayer::fillInstanceValidationLayerDebugInfo(vkInstanceCreateInfo,
                                                        debugCreateInfo);

  ABORT_ON_FAIL(vkCreateInstance(&vkInstanceCreateInfo, nullptr, &m_vkInstance),
                "Failed to create vulkan instance");

  return m_vkInstance;
}

VulkanInstance::~VulkanInstance() { vkDestroyInstance(m_vkInstance, nullptr); }

VkInstance VulkanInstance::getHandle() const { return m_vkInstance; }

}  // namespace engine
