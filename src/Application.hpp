#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>

#include "ApplicationConfig.hpp"
#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

using engine::ApplicationConfig;
using engine::ValidationLayerManager;
using engine::VulkanInstanceManager;
using engine::WindowManager;

class Application {
 public:
  Application();

  void run();

 private:
  std::unique_ptr<VulkanInstanceManager> m_vkInstanceManager = nullptr;
  std::unique_ptr<WindowManager> m_windowManager = nullptr;
  std::unique_ptr<ValidationLayerManager> m_validationLayerManager = nullptr;
};

#endif  // APPLICATION_HPP
