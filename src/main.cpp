#include <exception>
#include <iostream>

#include "ApplicationConfig.hpp"
#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

using engine::ApplicationConfig;

class Application {
 public:
  Application()
      : m_windowManager(ApplicationConfig::WINDOW_WIDTH,
                        ApplicationConfig::WINDOW_HEIGHT,
                        ApplicationConfig::WINDOW_TITLE),
        m_vkInstanceManager(ApplicationConfig::WINDOW_TITLE) {}

  void run() {
    while (m_windowManager.isOpen()) {
      m_windowManager.pollEvents();
    }
  }

 private:
  engine::WindowManager m_windowManager;
  engine::VulkanInstanceManager m_vkInstanceManager;
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
