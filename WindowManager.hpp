#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <cstdlib>
#include <vector>

namespace engine {

    class WindowManager {
    public:
        WindowManager(std::size_t width, std::size_t height, std::string title);

        virtual ~WindowManager();

        [[nodiscard]]
        bool isOpen() const;

        void pollEvents();

        static const char** getVulkanRequiredExtensions(std::uint32_t *extensionsCount);

    private:
        std::size_t m_width;
        std::size_t m_height;
        std::string m_title;
        GLFWwindow *m_window = nullptr;
    };

} // engine

#endif //WINDOW_MANAGER_HPP
