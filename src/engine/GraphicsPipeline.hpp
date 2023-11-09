#ifndef GRAPHICS_PIPELINE_HPP
#define GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class GraphicsPipeline {
 public:
  GraphicsPipeline(VkDevice device,
                   const VkGraphicsPipelineCreateInfo& pipelineInfo,
                   const VkAllocationCallbacks* alloc);

  virtual ~GraphicsPipeline();

  operator VkPipeline() {
    return m_graphicsPipeline;
  }

 private:
  VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // GRAPHICS_PIPELINE_HPP
