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
  WindowManager(std::size_t width, std::size_t height, std::string title);

  virtual ~WindowManager();

  [[nodiscard]] bool isOpen() const;

  void pollEvents();

  static std::vector<const char *> getRequiredExtensions();

 private:
  std::size_t m_width;
  std::size_t m_height;
  std::string m_title;
  GLFWwindow *m_window = nullptr;

  void centerWindow();
};

}  // namespace engine

#endif  // WINDOW_MANAGER_HPP
