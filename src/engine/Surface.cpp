#include "Surface.hpp"

#include <stdexcept>

#include "Abort.hpp"

namespace engine {
Surface::Surface(const VulkanInstance &instanceManager,
                 const Window &windowManager)
    : m_surface(nullptr), m_instanceManager(instanceManager) {
  VkInstance instance = instanceManager.getHandle();
  GLFWwindow *window = windowManager.getHandle();

  if (glfwCreateWindowSurface(instance, window, nullptr, &m_surface) !=
      VK_SUCCESS) {
    ABORT("Failed to create window surface");
  }
}

Surface::~Surface() {
  vkDestroySurfaceKHR(m_instanceManager.getHandle(), m_surface, nullptr);
}

VkSurfaceKHR Surface::getHandle() const { return m_surface; }
}  // namespace engine
