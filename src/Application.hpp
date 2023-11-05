#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "ApplicationConfig.hpp"
#include "GraphicsPipeline.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "RenderPass.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "ValidationLayerManager.hpp"
#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

using namespace engine;

class Application {
 public:
  Application();

  virtual ~Application();

  void run();

 private:
  WindowManager *m_windowManager;
  VulkanInstanceManager *m_vkInstanceManager;
  ValidationLayerManager *m_validationLayerManager;
  Surface *m_surface;
  PhysicalDeviceSelector *m_physicalDeviceSelector;
  LogicalDevice *m_logicalDevice;
  SwapChain *m_swapChain;
  RenderPass *m_renderPass;
  GraphicsPipeline *m_graphicsPipeline;
};

#endif  // APPLICATION_HPP
