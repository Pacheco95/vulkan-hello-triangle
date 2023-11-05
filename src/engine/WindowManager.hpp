#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define GLFW_INCLUDE_VULKAN
#pragma clang diagnostic pop

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <string>
#include <vector>

namespace engine {

class WindowManager {
 public:
  WindowManager(std::size_t width, std::size_t height,
                const std::string &title);

  virtual ~WindowManager();

  [[nodiscard]] bool isOpen() const;

  void pollEvents();

  static std::vector<const char *> getRequiredExtensions();

  [[nodiscard]] GLFWwindow *getHandle() const;

 private:
  GLFWwindow *m_window = nullptr;

  void centerWindow();
};

}  // namespace engine

#endif  // WINDOW_MANAGER_HPP
