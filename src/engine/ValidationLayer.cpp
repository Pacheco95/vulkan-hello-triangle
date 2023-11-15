#include "ValidationLayer.hpp"

#include "Abort.hpp"

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

[[maybe_unused]] VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger
) {
  return pfnVkCreateDebugUtilsMessengerEXT(
      instance, pCreateInfo, pAllocator, pMessenger
  );
}

[[maybe_unused]] VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const* pAllocator
) {
  return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

namespace engine {
uint32_t ValidationLayer::m_errorsCount = 0;

ValidationLayer::ValidationLayer(vk::Instance& instance)
    : m_instance(instance) {
  setupDebugMessenger();
}

ValidationLayer::~ValidationLayer() {
  m_instance.destroyDebugUtilsMessengerEXT(debugUtilsMessenger);
}

bool ValidationLayer::checkLayers(
    std::vector<char const*> const& layers,
    std::vector<vk::LayerProperties> const& properties
) {
  return std::all_of(
      layers.begin(),
      layers.end(),
      [&properties](auto layerName) {
        return std::any_of(
            properties.begin(),
            properties.end(),
            [&layerName](vk::LayerProperties const& property) {
              return strcmp(property.layerName, layerName) == 0;
            }
        );
      }
  );
}

void ValidationLayer::setupDebugMessenger() {
  pfnVkCreateDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
          m_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT")
      );

  if (!pfnVkCreateDebugUtilsMessengerEXT) {
    ABORT(
        "GetInstanceProcAddr: Unable to find "
        "pfnVkCreateDebugUtilsMessengerEXT function."
    );
  }

  pfnVkDestroyDebugUtilsMessengerEXT =
      reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
          m_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT")
      );

  if (!pfnVkDestroyDebugUtilsMessengerEXT) {
    ABORT(
        "GetInstanceProcAddr: Unable to find "
        "pfnVkDestroyDebugUtilsMessengerEXT function."
    );
  }

  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
  );

  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
  );

  vk::DebugUtilsMessengerCreateInfoEXT createInfo(
      {}, severityFlags, messageTypeFlags, &debugMessageFunc
  );

  debugUtilsMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);
}

VkBool32 ValidationLayer::debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void* /*pUserData*/
) {
  ++m_errorsCount;

  const std::string& severity = vk::to_string(
      static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)
  );

  const std::string& types =
      vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)
      );

  static constexpr char pad1[]{"    {:16} = {}\n"};
  static constexpr char pad2[]{"        {:12} = {}\n"};

  using namespace fmt;

  std::stringstream msg;

  msg << format("{}: {}\n", severity, types)
      << format(pad1, "messageIDName", pCallbackData->pMessageIdName)
      << format(pad1, "messageIdNumber", pCallbackData->messageIdNumber)
      << format(pad1, "message", pCallbackData->pMessage);

  if (pCallbackData->queueLabelCount > 0) {
    msg << "\tQueue Labels:\n";

    for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
      const auto& labelName = pCallbackData->pQueueLabels[i].pLabelName;
      msg << format(pad2, "labelName", labelName);
    }
  }

  if (pCallbackData->cmdBufLabelCount > 0) {
    msg << "\tCommandBuffer Labels:\n";

    for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
      msg << format(pad2, pCallbackData->pCmdBufLabels[i].pLabelName);
    }
  }

  for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
    const std::string& objType = vk::to_string(
        static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)
    );

    uint64_t objHandle = pCallbackData->pObjects[i].objectHandle;

    msg << format("\tObject {}:\n", i) << format(pad2, "objectType", objType)
        << format(pad2, "objectHandle", fmt::ptr((void*)objHandle));

    if (pCallbackData->pObjects[i].pObjectName) {
      msg << format(pad2, "objectName", pCallbackData->pObjects[i].pObjectName);
    }
  }


  SPDLOG_ERROR(msg.str());

  return false;
}

uint32_t ValidationLayer::getErrorsCount() { return m_errorsCount; }
}  // namespace engine
