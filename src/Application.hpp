#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "ApplicationConfig.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "Surface.hpp"
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
  WindowManager* m_windowManager;
  VulkanInstanceManager* m_vkInstanceManager;
  ValidationLayerManager* m_validationLayerManager;
  Surface* m_surface;
  PhysicalDeviceSelector* m_physicalDeviceSelector;
  LogicalDevice* m_logicalDevice;
};

#endif  // APPLICATION_HPP
