#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vulkan/vulkan.h>

#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

namespace engine {

class Surface {
 public:
  explicit Surface(const VulkanInstanceManager &instanceManager,
                   const WindowManager &windowManager);

  virtual ~Surface();

 private:
  VkSurfaceKHR m_surface;
  const VulkanInstanceManager &m_instanceManager;
};

}  // namespace engine

#endif  // SURFACE_HPP
