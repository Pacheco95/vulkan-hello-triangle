#include "Application.hpp"

using Buffer = ShaderLoader::Buffer;

Application::Application() {
  m_windowManager = new Window(ApplicationConfig::WINDOW_WIDTH,
                               ApplicationConfig::WINDOW_HEIGHT,
                               ApplicationConfig::WINDOW_TITLE);

  m_vkInstanceManager = new VulkanInstance(ApplicationConfig::APP_NAME);

  m_validationLayerManager =
      new ValidationLayer(m_vkInstanceManager->getHandle());

  m_surface = new Surface(*m_vkInstanceManager, *m_windowManager);

  m_physicalDeviceSelector = new PhysicalDeviceSelector(
      m_vkInstanceManager->getHandle(), m_surface->getHandle());

  m_logicalDevice = new LogicalDevice(m_physicalDeviceSelector->getHandle(),
                                      m_surface->getHandle());

  VkDevice device = m_logicalDevice->getHandle();

  m_swapChain = new SwapChain(m_windowManager->getHandle(),
                              m_physicalDeviceSelector->getHandle(), device,
                              m_surface->getHandle());

  m_renderPass = new RenderPass(device, *m_swapChain);

  Buffer fragShaderByteCode = ShaderLoader::load("res/shaders/frag.spv");
  Buffer vertShaderByteCode = ShaderLoader::load("res/shaders/vert.spv");

  m_graphicsPipeline =
      new GraphicsPipeline(device, vertShaderByteCode, fragShaderByteCode,
                           *m_swapChain, *m_renderPass);
}

void Application::run() {
  while (m_windowManager->isOpen()) {
    m_windowManager->pollEvents();
  }
}

Application::~Application() {
  delete m_graphicsPipeline;
  delete m_renderPass;
  delete m_swapChain;
  delete m_logicalDevice;
  delete m_physicalDeviceSelector;
  delete m_surface;
  delete m_validationLayerManager;
  delete m_vkInstanceManager;
  delete m_windowManager;
}
