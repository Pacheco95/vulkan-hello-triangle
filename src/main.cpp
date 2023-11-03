#include <exception>
#include <iostream>
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
  Application() {
    m_vkInstanceManager =
        std::make_unique<VulkanInstanceManager>(ApplicationConfig::APP_NAME);

    m_windowManager = std::make_unique<WindowManager>(
        ApplicationConfig::WINDOW_WIDTH, ApplicationConfig::WINDOW_HEIGHT,
        ApplicationConfig::WINDOW_TITLE);

    m_validationLayerManager = std::make_unique<ValidationLayerManager>(
        m_vkInstanceManager->getInstance());
  }

  void run() {
    while (m_windowManager->isOpen()) {
      m_windowManager->pollEvents();
    }
  }

 private:
  std::unique_ptr<VulkanInstanceManager> m_vkInstanceManager = nullptr;
  std::unique_ptr<WindowManager> m_windowManager = nullptr;
  std::unique_ptr<ValidationLayerManager> m_validationLayerManager = nullptr;
};

int main() {
  Application app{};

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
