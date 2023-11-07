#include "Application.hpp"

using Buffer = ShaderLoader::Buffer;

Application::Application() {
  m_window = new Window(ApplicationConfig::WINDOW_WIDTH,
                        ApplicationConfig::WINDOW_HEIGHT,
                        ApplicationConfig::WINDOW_TITLE);

  m_vkInstance = new VulkanInstance(ApplicationConfig::APP_NAME);

  m_validationLayer = new ValidationLayer(m_vkInstance->getHandle());

  m_surface = new Surface(*m_vkInstance, *m_window);

  m_physicalDeviceSelector = new PhysicalDeviceSelector(
      m_vkInstance->getHandle(), m_surface->getHandle());

  m_logicalDevice = new LogicalDevice(m_physicalDeviceSelector->getHandle(),
                                      m_surface->getHandle());

  VkDevice device = m_logicalDevice->getHandle();

  m_swapChain = new SwapChain(m_window->getHandle(),
                              m_physicalDeviceSelector->getHandle(), device,
                              m_surface->getHandle());

  m_renderPass = new RenderPass(device, *m_swapChain);

  Buffer fragShaderByteCode = ShaderLoader::load("res/shaders/frag.spv");
  Buffer vertShaderByteCode = ShaderLoader::load("res/shaders/vert.spv");

  m_graphicsPipeline =
      new GraphicsPipeline(device, vertShaderByteCode, fragShaderByteCode,
                           *m_swapChain, *m_renderPass);

  m_frameBuffers = FrameBuffer::getFromImageViews(
      m_swapChain->getImageViews(), m_renderPass->getHandle(),
      m_swapChain->getSwapChainExtent(), m_logicalDevice->getHandle());
}

void Application::run() {
  while (m_window->isOpen()) {
    m_window->pollEvents();
  }
}

Application::~Application() {
  m_frameBuffers.clear();
  delete m_graphicsPipeline;
  delete m_renderPass;
  delete m_swapChain;
  delete m_logicalDevice;
  delete m_physicalDeviceSelector;
  delete m_surface;
  delete m_validationLayer;
  delete m_vkInstance;
  delete m_window;
}
