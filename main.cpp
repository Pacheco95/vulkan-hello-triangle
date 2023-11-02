#include <exception>
#include <iostream>

#include "VulkanInstanceManager.hpp"
#include "WindowManager.hpp"

const std::size_t WINDOW_WIDTH = 800;
const std::size_t WINDOW_HEIGHT = 600;

class Application {
public:
    Application()
            : m_windowManager(WINDOW_WIDTH, WINDOW_HEIGHT, m_name),
              m_vkInstanceManager(m_name) {
    }

    void run() {
        while (m_windowManager.isOpen()) {
            m_windowManager.pollEvents();
        }
    }

private:
    engine::WindowManager m_windowManager;
    engine::VulkanInstanceManager m_vkInstanceManager;
    const static char *m_name;
};

const char *Application::m_name = "Hello Triangle";


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