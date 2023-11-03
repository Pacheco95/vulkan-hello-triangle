#include <stdexcept>
#include <cstring>
#include "ApplicationConfig.hpp"
#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

namespace engine {
    VulkanInstanceManager::VulkanInstanceManager(const std::string &applicationName)
            : m_instance(nullptr),
              m_applicationName(applicationName) {

        if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED && !checkValidationLayerSupport()) {
            throw std::runtime_error("One or more validation layers requested not available");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = m_applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        std::uint32_t extensionsCount;
        const char** requiredExtensions = WindowManager::getVulkanRequiredExtensions(&extensionsCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = requiredExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan instance");
        }
    }

    VulkanInstanceManager::~VulkanInstanceManager() {
        vkDestroyInstance(m_instance, nullptr);
    }

    bool VulkanInstanceManager::checkValidationLayerSupport() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : ApplicationConfig::VALIDATION_LAYERS) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
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

} // engine