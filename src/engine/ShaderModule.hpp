#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class ShaderModule {
 public:
  ShaderModule(VkDevice device, const VkShaderModuleCreateInfo& createInfo,
               const VkAllocationCallbacks* alloc);

  virtual ~ShaderModule();

  operator VkShaderModule() {return m_shaderModule; }
 private:
  VkDevice m_device;
  VkShaderModule m_shaderModule = VK_NULL_HANDLE;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // SHADER_MODULE_HPP
