#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "ShaderLoader.hpp"

namespace engine {
using ByteCode = const ShaderLoader::Buffer&;

class GraphicsPipeline {
 public:
  GraphicsPipeline(VkDevice device, ByteCode vertShaderByteCode,
                   ByteCode fragShaderByteCode, VkExtent2D swapChainExtent);

  virtual ~GraphicsPipeline();

 private:
  VkDevice m_device;
  VkPipelineLayout m_pipelineLayout;

  static VkShaderModule createShaderModule(ByteCode ShaderByteCode,
                                           VkDevice device);
};

}  // namespace engine

#endif  // GRAPHICS_PIPELINE_HPP
