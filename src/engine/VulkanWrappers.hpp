/**
 * This file contains definitions of vulkan types wrapped in a RAII-based class
 */

#ifndef VULKAN_WRAPPERS_HPP
#define VULKAN_WRAPPERS_HPP

#include "VkWrapper.hpp"

/**
 * vkCreateGraphicsPipelines contains more parameters than other "VkCreate"
 * functions. In order to wrap vkCreateGraphicsPipelines using VkWrapper some
 * parameter should be default initialized. Note that this function creates only
 * one pipeline at a time.
 */
VKAPI_ATTR VkResult VKAPI_CALL createPipeline(
    VkDevice device,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines
) {
  return vkCreateGraphicsPipelines(
      device, VK_NULL_HANDLE, 1, pCreateInfos, pAllocator, pPipelines
  );
}

using ImageView = VkWrapper<
    VkImageView,
    VkImageViewCreateInfo,
    vkCreateImageView,
    vkDestroyImageView>;

using FrameBuffer = VkWrapper<
    VkFramebuffer,
    VkFramebufferCreateInfo,
    vkCreateFramebuffer,
    vkDestroyFramebuffer>;

using RenderPass = VkWrapper<
    VkRenderPass,
    VkRenderPassCreateInfo,
    vkCreateRenderPass,
    vkDestroyRenderPass>;

using Semaphore = VkWrapper<
    VkSemaphore,
    VkSemaphoreCreateInfo,
    vkCreateSemaphore,
    vkDestroySemaphore>;

using ShaderModule = VkWrapper<
    VkShaderModule,
    VkShaderModuleCreateInfo,
    vkCreateShaderModule,
    vkDestroyShaderModule>;

using SwapChain = VkWrapper<
    VkSwapchainKHR,
    VkSwapchainCreateInfoKHR,
    vkCreateSwapchainKHR,
    vkDestroySwapchainKHR>;

using Fence =
    VkWrapper<VkFence, VkFenceCreateInfo, vkCreateFence, vkDestroyFence>;

using PipelineLayout = VkWrapper<
    VkPipelineLayout,
    VkPipelineLayoutCreateInfo,
    vkCreatePipelineLayout,
    vkDestroyPipelineLayout>;

using GraphicsPipeline = VkWrapper<
    VkPipeline,
    VkGraphicsPipelineCreateInfo,
    createPipeline,
    vkDestroyPipeline>;

using CommandPool = VkWrapper<
    VkCommandPool,
    VkCommandPoolCreateInfo,
    vkCreateCommandPool,
    vkDestroyCommandPool>;

using Buffer =
    VkWrapper<VkBuffer, VkBufferCreateInfo, vkCreateBuffer, vkDestroyBuffer>;

using DeviceMemory = VkWrapper<
    VkDeviceMemory,
    VkMemoryAllocateInfo,
    vkAllocateMemory,
    vkFreeMemory>;

using DescriptorSetLayout = VkWrapper<
    VkDescriptorSetLayout,
    VkDescriptorSetLayoutCreateInfo,
    vkCreateDescriptorSetLayout,
    vkDestroyDescriptorSetLayout>;

using DescriptorPool = VkWrapper<
    VkDescriptorPool,
    VkDescriptorPoolCreateInfo,
    vkCreateDescriptorPool,
    vkDestroyDescriptorPool>;

using Image =
    VkWrapper<VkImage, VkImageCreateInfo, vkCreateImage, vkDestroyImage>;

using Sampler = VkWrapper<
    VkSampler,
    VkSamplerCreateInfo,
    vkCreateSampler,
    vkDestroySampler>;

#endif  // VULKAN_WRAPPERS_HPP
