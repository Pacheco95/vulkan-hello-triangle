#ifndef VULKAN_INSTANCE_MANAGER_HPP
#define VULKAN_INSTANCE_MANAGER_HPP

#include <vulkan/vulkan.h>
#include <string>

namespace engine {

    class VulkanInstanceManager {
    public:
        explicit VulkanInstanceManager(const std::string &applicationName);

        virtual ~VulkanInstanceManager();

    private:
        VkInstance m_instance;
        std::string m_applicationName;
    };

}

#endif //VULKAN_INSTANCE_MANAGER_HPP
