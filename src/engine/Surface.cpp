#include "Surface.hpp"

#include <stdexcept>

#include "Abort.hpp"

namespace engine {
Surface::Surface(const VulkanInstance &vkInstance, const Window &window)
    : m_surface(nullptr), m_instance(vkInstance) {
  if (glfwCreateWindowSurface(vkInstance.getHandle(), window.getHandle(),
                              nullptr, &m_surface) != VK_SUCCESS) {
    ABORT("Failed to create window surface");
  }
}

Surface::~Surface() {
  vkDestroySurfaceKHR(m_instance.getHandle(), m_surface, nullptr);
}

VkSurfaceKHR Surface::getHandle() const { return m_surface; }
}  // namespace engine
