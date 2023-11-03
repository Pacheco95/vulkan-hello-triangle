#include "Application.hpp"

Application::Application() {
  m_vkInstanceManager =
      std::make_unique<VulkanInstanceManager>(ApplicationConfig::APP_NAME);

  m_windowManager = std::make_unique<WindowManager>(
      ApplicationConfig::WINDOW_WIDTH, ApplicationConfig::WINDOW_HEIGHT,
      ApplicationConfig::WINDOW_TITLE);

  m_validationLayerManager = std::make_unique<ValidationLayerManager>(
      m_vkInstanceManager->getInstance());

  m_physicalDeviceSelector = std::make_unique<PhysicalDeviceSelector>(
      m_vkInstanceManager->getInstance());
}

void Application::run() {
  while (m_windowManager->isOpen()) {
    m_windowManager->pollEvents();
  }
}
