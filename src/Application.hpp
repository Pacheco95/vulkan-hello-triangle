#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "ApplicationConfig.hpp"
#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "RenderPass.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "ValidationLayer.hpp"
#include "VulkanInstance.hpp"
#include "Window.hpp"

using namespace engine;

class Application {
 public:
  Application();

  virtual ~Application();

  void run();

 private:
  Window *m_window;
  VulkanInstance *m_vkInstance;
  ValidationLayer *m_validationLayer;
  Surface *m_surface;
  PhysicalDeviceSelector *m_physicalDeviceSelector;
  LogicalDevice *m_logicalDevice;
  SwapChain *m_swapChain;
  RenderPass *m_renderPass;
  GraphicsPipeline *m_graphicsPipeline;
  std::vector<FrameBuffer> m_frameBuffers;
};

#endif  // APPLICATION_HPP
