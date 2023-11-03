#include "VulkanInstanceManager.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "ApplicationConfig.hpp"
#include "WindowManager.hpp"

namespace engine {
using engine::ValidationLayerManager;

[[maybe_unused]] VulkanInstanceManager::VulkanInstanceManager(
    std::string applicationName)
    : m_instance(createInstance()),
      m_applicationName(std::move(applicationName)) {}

VkInstance VulkanInstanceManager::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = m_applicationName.c_str();
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
    throw std::runtime_error("Failed to create vulkan instance");
  }

  return m_instance;
}

VulkanInstanceManager::~VulkanInstanceManager() {
  vkDestroyInstance(m_instance, nullptr);
}

VkInstance VulkanInstanceManager::getInstance() { return m_instance; }

}  // namespace engine
