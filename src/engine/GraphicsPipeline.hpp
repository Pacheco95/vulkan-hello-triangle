#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "RenderPass.hpp"
#include "ShaderLoader.hpp"
#include "SwapChain.hpp"

namespace engine {
using ByteCode = const ShaderLoader::Buffer&;
using ShaderStages = std::array<VkPipelineShaderStageCreateInfo, 2>;

class GraphicsPipeline {
 public:
  GraphicsPipeline(VkDevice device, ByteCode vertShaderByteCode,
                   ByteCode fragShaderByteCode,
                   const engine::SwapChain& swapChain,
                   const engine::RenderPass& renderPass);

  virtual ~GraphicsPipeline();

 private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;

  static VkShaderModule createShaderModule(ByteCode ShaderByteCode,
                                           VkDevice device);

  static ShaderStages getShaderStages(VkShaderModule vertShaderModule,
                                      VkShaderModule fragShaderModule);
  void createPipeline(const SwapChain& swapChain, ShaderStages& shaderStages,
                      VkRenderPass renderPass);
};

}  // namespace engine

#endif  // GRAPHICS_PIPELINE_HPP
