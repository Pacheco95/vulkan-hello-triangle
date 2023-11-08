#ifndef PIPELINE_LAYOUT_HPP
#define PIPELINE_LAYOUT_HPP

#include <vulkan/vulkan.h>

#include "Abort.hpp"

namespace engine {

class PipelineLayout {
 public:
  PipelineLayout(VkDevice device,
                 const VkPipelineLayoutCreateInfo& pipelineLayoutInfo,
                 const VkAllocationCallbacks* alloc);

  virtual ~PipelineLayout();

  VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
  VkDevice m_device;
  const VkAllocationCallbacks* m_alloc;
};

}  // namespace engine

#endif  // PIPELINE_LAYOUT_HPP
