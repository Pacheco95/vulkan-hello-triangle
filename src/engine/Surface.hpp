#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vulkan/vulkan.h>

#include "VulkanInstance.hpp"
#include "Window.hpp"

namespace engine {

class Surface {
 public:
  explicit Surface(const VulkanInstance &vkInstance, const Window &window);

  virtual ~Surface();

  [[nodiscard]] VkSurfaceKHR getHandle() const;

 private:
  VkSurfaceKHR m_surface;
  const VulkanInstance &m_instance;
};

}  // namespace engine

#endif  // SURFACE_HPP
