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

  VkPhysicalDevice physicalDevice =
      m_physicalDeviceSelector->getSelectedDevice();

  m_logicalDevice = new LogicalDevice(physicalDevice, m_surface->getSurface());

  VkDevice device = m_logicalDevice->getDevice();

  m_swapChain = new SwapChain(m_windowManager->getWindow(), physicalDevice,
                              device, m_surface->getSurface());

  Buffer fragShaderByteCode = ShaderLoader::load("res/shaders/frag.spv");
  Buffer vertShaderByteCode = ShaderLoader::load("res/shaders/vert.spv");

  m_renderPass = new RenderPass(device, *m_swapChain);

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
