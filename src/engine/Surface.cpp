#include "Surface.hpp"

#include <stdexcept>

#include "Abort.hpp"

namespace engine {
Surface::Surface(const VulkanInstanceManager &instanceManager,
                 const WindowManager &windowManager)
    : m_surface(nullptr), m_instanceManager(instanceManager) {
  VkInstance instance = instanceManager.getInstance();
  GLFWwindow *window = windowManager.getWindow();

  if (glfwCreateWindowSurface(instance, window, nullptr, &m_surface) !=
      VK_SUCCESS) {
    ABORT("Failed to create window surface");
  }
}

Surface::~Surface() {
  vkDestroySurfaceKHR(m_instanceManager.getInstance(), m_surface, nullptr);
}

VkSurfaceKHR Surface::getSurface() const { return m_surface; }
}  // namespace engine
