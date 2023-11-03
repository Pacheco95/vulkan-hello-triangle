#include "WindowManager.hpp"

#include <stdexcept>

#include "ApplicationConfig.hpp"

namespace engine {
WindowManager::WindowManager(size_t width, size_t height, std::string title)
    : m_width(width), m_height(height), m_title(title) {
  const char *initErrorMessage = "Failed to create window";

  if (!glfwInit()) {
    throw std::runtime_error(initErrorMessage);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_window =
      glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height),
                       m_title.c_str(), nullptr, nullptr);

  if (!m_window) {
    throw std::runtime_error(initErrorMessage);
  }

  centerWindow();
}

WindowManager::~WindowManager() {
  if (m_window) {
    glfwDestroyWindow(m_window);
  }

  glfwTerminate();
}

bool WindowManager::isOpen() const { return !glfwWindowShouldClose(m_window); }

void WindowManager::pollEvents() { glfwPollEvents(); }

std::vector<const char *> WindowManager::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (ApplicationConfig::IS_VALIDATION_LAYERS_ENABLED) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void WindowManager::centerWindow() {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  int xPos = (mode->width - 800) / 2;
  int yPos = (mode->height - 600) / 2;
  glfwSetWindowPos(m_window, xPos, yPos);
}

}  // namespace engine
