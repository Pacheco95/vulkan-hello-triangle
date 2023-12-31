#ifndef WINDOW_HPP
#define WINDOW_HPP

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define GLFW_INCLUDE_VULKAN
#pragma clang diagnostic pop

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "Config.hpp"

namespace engine {

class Window {
 public:
  Window(std::size_t width, std::size_t height, const std::string &title);

  virtual ~Window();

  [[nodiscard]] bool isOpen() const;

  void pollEvents();

  operator GLFWwindow *() { return m_window; }

  static std::vector<const char *> getRequiredExtensions();

 private:
  GLFWwindow *m_window = nullptr;

  void centerWindow();
};

}  // namespace engine

#endif  // WINDOW_HPP
