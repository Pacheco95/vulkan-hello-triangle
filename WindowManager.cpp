#include <stdexcept>
#include "WindowManager.hpp"

namespace engine {
    WindowManager::WindowManager(size_t width, size_t height, std::string title)
            : m_width(width), m_height(height),
              m_title(title) {

        const char *initErrorMessage = "Failed to create window";

        if (!glfwInit()) {
            throw std::runtime_error(initErrorMessage);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(
                static_cast<int>(m_width),
                static_cast<int>(m_height),
                m_title.c_str(),
                nullptr,
                nullptr
        );

        if (!m_window) {
            throw std::runtime_error(initErrorMessage);
        }
    }

    WindowManager::~WindowManager() {
        if (m_window) {
            glfwDestroyWindow(m_window);
        }

        glfwTerminate();
    }

    bool WindowManager::isOpen() const {
        return !glfwWindowShouldClose(m_window);
    }

    void WindowManager::pollEvents() {
        glfwPollEvents();
    }

    const char **WindowManager::getVulkanRequiredExtensions(std::uint32_t *extensionsCount) {
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(extensionsCount);
        return glfwExtensions;
    }

} // engine