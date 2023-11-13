#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

class Camera {
 public:
  explicit Camera(GLFWwindow* window);

  [[nodiscard]] glm::mat4 getViewMatrix() const;

  void update(float deltaTime);

  void setActive(bool active);

 private:
  GLFWwindow* window;
  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  float yaw = -90.0f;
  float pitch = 0.0f;
  float lastX;
  float lastY;
  bool firstMouse = true;
  float sensitivity = 3.0f;
  float speed = 1.0f;
  bool m_active = false;

  void move(float deltaTime);

  void updateVectors(float deltaTime);
};

}  // namespace engine

#endif  // CAMERA_HPP
