#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "ShaderLoader.hpp"
#include "SwapChain.hpp"

namespace engine {
using ByteCode = const ShaderLoader::Buffer&;

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

  static VkShaderModule createShaderModule(ByteCode ShaderByteCode,
                                           VkDevice device);

  void createRenderPass(VkFormat swapChainImageFormat);

  void createPipelineLayout(const SwapChain& swapChain,
                            VkShaderModule vertShaderModule,
                            VkShaderModule fragShaderModule);
};

}  // namespace engine

#endif  // GRAPHICS_PIPELINE_HPP
