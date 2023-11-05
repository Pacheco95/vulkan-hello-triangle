#include "VulkanInstanceManager.hpp"

#include <stdexcept>

#include "Abort.hpp"
#include "ApplicationConfig.hpp"
#include "WindowManager.hpp"

namespace engine {
using engine::ValidationLayerManager;

[[maybe_unused]] VulkanInstanceManager::VulkanInstanceManager(
    const std::string &applicationName)
    : m_instance(createInstance(applicationName)) {}

VkInstance VulkanInstanceManager::createInstance(
    const std::string &applicationName) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = applicationName.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  std::vector<const char *> requiredExtensions =
      WindowManager::getRequiredExtensions();

  VkInstanceCreateInfo vkInstanceCreateInfo{};
  vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkInstanceCreateInfo.pApplicationInfo = &appInfo;
  vkInstanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
  vkInstanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
      ValidationLayerManager::getDebugMessengerCreateInfo();

  ValidationLayerManager::fillInstanceValidationLayerDebugInfo(
      vkInstanceCreateInfo, debugCreateInfo);

  if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &m_instance) !=
      VK_SUCCESS) {
    ABORT("Failed to create vulkan instance");
  }

  return m_instance;
}

VulkanInstanceManager::~VulkanInstanceManager() {
  vkDestroyInstance(m_instance, nullptr);
}

VkInstance VulkanInstanceManager::getHandle() const { return m_instance; }

}  // namespace engine
