#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "ShaderLoader.hpp"
#include "SwapChain.hpp"

namespace engine {
using ByteCode = const ShaderLoader::Buffer&;
using ShaderStages = std::array<VkPipelineShaderStageCreateInfo, 2>;

class GraphicsPipeline {
 public:
  GraphicsPipeline(VkDevice device, ByteCode vertShaderByteCode,
                   ByteCode fragShaderByteCode,
                   const engine::SwapChain& swapChain);

  virtual ~GraphicsPipeline();

 private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;
  VkRenderPass renderPass;
  VkPipeline m_graphicsPipeline;

  static VkShaderModule createShaderModule(ByteCode ShaderByteCode,
                                           VkDevice device);

  void createRenderPass(VkFormat swapChainImageFormat);

  static ShaderStages getShaderStages(VkShaderModule vertShaderModule,
                                      VkShaderModule fragShaderModule);
  void createPipeline(const SwapChain& swapChain, ShaderStages& shaderStages);
};

}  // namespace engine

#endif  // GRAPHICS_PIPELINE_HPP
