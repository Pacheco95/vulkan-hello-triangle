#include "ShaderModule.hpp"

namespace engine {
ShaderModule::ShaderModule(VkDevice device,
                           const VkShaderModuleCreateInfo &createInfo,
                           const VkAllocationCallbacks *alloc)
    : m_device(device), m_alloc(alloc) {
  ABORT_ON_FAIL(
      vkCreateShaderModule(device, &createInfo, alloc, &m_shaderModule),
      "Failed to create shader module");

  SPDLOG_DEBUG("Created shader module: {}", fmt::ptr(m_shaderModule));
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(m_device, m_shaderModule, m_alloc);
  SPDLOG_DEBUG("Destroyed shader module: {}", fmt::ptr(m_shaderModule));
}

VkShaderModule ShaderModule::getHandle() const { return m_shaderModule; }
}  // namespace engine
