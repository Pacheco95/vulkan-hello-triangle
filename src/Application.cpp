#include "Application.hpp"

using Buffer = ShaderLoader::Buffer;

Application::Application() {
  m_windowManager = new WindowManager(ApplicationConfig::WINDOW_WIDTH,
                                      ApplicationConfig::WINDOW_HEIGHT,
                                      ApplicationConfig::WINDOW_TITLE);

  m_vkInstanceManager = new VulkanInstanceManager(ApplicationConfig::APP_NAME);

  m_validationLayerManager =
      new ValidationLayerManager(m_vkInstanceManager->getInstance());

  m_surface = new Surface(*m_vkInstanceManager, *m_windowManager);

  m_physicalDeviceSelector = new PhysicalDeviceSelector(
      m_vkInstanceManager->getInstance(), m_surface->getSurface());

  m_logicalDevice = new LogicalDevice(
      m_physicalDeviceSelector->getSelectedDevice(), m_surface->getSurface());

  m_swapChain =
      new SwapChain(m_windowManager->getWindow(),
                    m_physicalDeviceSelector->getSelectedDevice(),
                    m_logicalDevice->getDevice(), m_surface->getSurface());

  Buffer fragShaderByteCode = ShaderLoader::load("res/shaders/frag.spv");
  Buffer vertShaderByteCode = ShaderLoader::load("res/shaders/vert.spv");

  m_graphicsPipeline =
      new GraphicsPipeline(m_logicalDevice->getDevice(), vertShaderByteCode,
                           fragShaderByteCode, *m_swapChain);
}

void Application::run() {
  while (m_windowManager->isOpen()) {
    m_windowManager->pollEvents();
  }
}

Application::~Application() {
  delete m_graphicsPipeline;
  delete m_swapChain;
  delete m_logicalDevice;
  delete m_physicalDeviceSelector;
  delete m_surface;
  delete m_validationLayerManager;
  delete m_vkInstanceManager;
  delete m_windowManager;
}
