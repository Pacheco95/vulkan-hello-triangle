#include "Application.hpp"

Application::Application() {
  m_windowManager = new WindowManager(ApplicationConfig::WINDOW_WIDTH,
                                      ApplicationConfig::WINDOW_HEIGHT,
                                      ApplicationConfig::WINDOW_TITLE);

  m_vkInstanceManager = new VulkanInstanceManager(ApplicationConfig::APP_NAME);

  m_validationLayerManager =
      new ValidationLayerManager(m_vkInstanceManager->getInstance());

  m_surface = new Surface(*m_vkInstanceManager, *m_windowManager);

  m_physicalDeviceSelector =
      new PhysicalDeviceSelector(m_vkInstanceManager->getInstance());

  m_logicalDevice =
      new LogicalDevice(m_physicalDeviceSelector->getSelectedDevice());
}

void Application::run() {
  while (m_windowManager->isOpen()) {
    m_windowManager->pollEvents();
  }
}

Application::~Application() {
  delete m_logicalDevice;
  delete m_physicalDeviceSelector;
  delete m_surface;
  delete m_validationLayerManager;
  delete m_vkInstanceManager;
  delete m_windowManager;
}