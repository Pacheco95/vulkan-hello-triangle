#include "ValidationLayerManager.hpp"

#include <cstring>
#include <iostream>

#include "ApplicationConfig.hpp"

namespace engine {
ValidationLayerManager::ValidationLayerManager(VkInstance instance)
    : m_instance(instance), m_debugMessenger(nullptr) {
  if (!ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    return;
  }

  if (!checkValidationLayerSupport()) {
    throw std::runtime_error(
        "One or more validation layers requested not available");
  }

  auto createInfo = getDebugMessengerCreateInfo();
  if (createDebugUtilsMessengerEXT(m_instance, &createInfo,

                                   &m_debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("Failed to set up debug messenger!");
  }
}

ValidationLayerManager::~ValidationLayerManager() {
  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerManager::debugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT _messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    [[maybe_unused]] void *_pUserData) {
  std::cerr << "[VALIDATION-LAYER]: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT
ValidationLayerManager::getDebugMessengerCreateInfo() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};

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

bool ValidationLayerManager::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : engine::ApplicationConfig::VALIDATION_LAYERS) {
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

VkResult ValidationLayerManager::createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (!func) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return func(instance, pCreateInfo, nullptr, pDebugMessenger);
}

void ValidationLayerManager::destroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debugMessenger, nullptr);
  }
}

void ValidationLayerManager::fillInstanceValidationLayerDebugInfo(
    VkInstanceCreateInfo &vkInstanceCreateInfo,
    const VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo) {
  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    vkInstanceCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(ApplicationConfig::VALIDATION_LAYERS.size());
    vkInstanceCreateInfo.ppEnabledLayerNames =
        ApplicationConfig::VALIDATION_LAYERS.data();
    vkInstanceCreateInfo.pNext = &debugCreateInfo;
  }
}
}  // namespace engine
