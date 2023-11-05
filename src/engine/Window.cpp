#include "Window.hpp"

#include <stdexcept>

#include "Abort.hpp"
#include "ApplicationConfig.hpp"

namespace engine {
Window::Window(size_t width, size_t height, const std::string &title) {
  const char *initErrorMessage = "Failed to create window";

  if (!glfwInit()) {
    ABORT(initErrorMessage);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                              title.c_str(), nullptr, nullptr);

  if (!m_window) {
    ABORT(initErrorMessage);
  }

  centerWindow();
}

Window::~Window() {
  if (m_window) {
    glfwDestroyWindow(m_window);
  }

  glfwTerminate();
}

bool Window::isOpen() const {
  bool shouldClose = glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
  shouldClose = shouldClose || glfwWindowShouldClose(m_window);
  return !shouldClose;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"

void Window::pollEvents() { glfwPollEvents(); }

#pragma clang diagnostic pop

std::vector<const char *> Window::getRequiredExtensions() {
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

void Window::centerWindow() {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  int xPos = (mode->width - 800) / 2;
  int yPos = (mode->height - 600) / 2;
  glfwSetWindowPos(m_window, xPos, yPos);
}

GLFWwindow *Window::getHandle() const { return m_window; }

}  // namespace engine
