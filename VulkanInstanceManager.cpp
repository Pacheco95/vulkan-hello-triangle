#include "VulkanInstanceManager.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "ApplicationConfig.hpp"
#include "WindowManager.hpp"

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (!func) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debugMessenger, pAllocator);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  std::cerr << "[VALIDATION-LAYER]: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

  createInfo.pfnUserCallback = debugCallback;

  return createInfo;
}

namespace engine {
VulkanInstanceManager::VulkanInstanceManager(const std::string &applicationName)
    : m_instance(nullptr),
      m_applicationName(applicationName),
      m_debugMessenger(nullptr) {
  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED &&
      !checkValidationLayerSupport()) {
    throw std::runtime_error(
        "One or more validation layers requested not available");
  }

  createInstance();

  setupDebugMessenger();
}

void VulkanInstanceManager::createInstance() {
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
      getDebugMessengerCreateInfo();

  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    vkInstanceCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(ApplicationConfig::VALIDATION_LAYERS.size());
    vkInstanceCreateInfo.ppEnabledLayerNames =
        ApplicationConfig::VALIDATION_LAYERS.data();
    vkInstanceCreateInfo.pNext = &debugCreateInfo;
  }

  if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &m_instance) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan instance");
  }
}

VulkanInstanceManager::~VulkanInstanceManager() {
  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
  }

  vkDestroyInstance(m_instance, nullptr);
}

bool VulkanInstanceManager::checkValidationLayerSupport() const {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : ApplicationConfig::VALIDATION_LAYERS) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

void VulkanInstanceManager::setupDebugMessenger() {
  if (!ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();

  if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr,

                                   &m_debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("Failed to set up debug messenger!");
  }
}

}  // namespace engine
