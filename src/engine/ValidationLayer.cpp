#include "ValidationLayer.hpp"

#include <cstring>
#include <iostream>

#include "Abort.hpp"
#include "ApplicationConfig.hpp"

namespace engine {
ValidationLayer::ValidationLayer(VkInstance instance)
    : m_vkInstance(instance), m_debugMessenger(nullptr) {
  if (!ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    return;
  }

  if (!checkValidationLayerSupport()) {
    ABORT("One or more validation layers requested not available");
  }

  auto createInfo = getDebugMessengerCreateInfo();

  ABORT_ON_FAIL(createDebugUtilsMessengerEXT(m_vkInstance, &createInfo,
                                             &m_debugMessenger),
                "Failed to set up debug messenger");
}

ValidationLayer::~ValidationLayer() {
  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    destroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayer::debugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT _messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    [[maybe_unused]] void *_pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      SPDLOG_DEBUG(pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      SPDLOG_WARN(pCallbackData->pMessage);
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      SPDLOG_ERROR(pCallbackData->pMessage);
  }

  return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT
ValidationLayer::getDebugMessengerCreateInfo() {
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

bool ValidationLayer::checkValidationLayerSupport() {
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

VkResult ValidationLayer::createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (!func) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return func(instance, pCreateInfo, nullptr, pDebugMessenger);
}

void ValidationLayer::destroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");

  if (func) {
    func(instance, debugMessenger, nullptr);
  }
}

void ValidationLayer::fillInstanceValidationLayerDebugInfo(
    VkInstanceCreateInfo &vkInstanceCreateInfo,
    const VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo) {
  if (!ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    return;
  }

  vkInstanceCreateInfo.enabledLayerCount =
      static_cast<uint32_t>(ApplicationConfig::VALIDATION_LAYERS.size());

  vkInstanceCreateInfo.ppEnabledLayerNames =
      ApplicationConfig::VALIDATION_LAYERS.data();

  vkInstanceCreateInfo.pNext = &debugCreateInfo;
}
}  // namespace engine
