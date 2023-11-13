#include "Camera.hpp"

namespace engine {
Camera::Camera(GLFWwindow *window) : window(window) {
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  lastX = static_cast<float>(windowWidth) / 2.0f;
  lastY = static_cast<float>(windowHeight) / 2.0f;
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::update(float deltaTime) {
  if (!m_active) {
    return;
  }

  move(deltaTime);
  updateVectors(deltaTime);
}

void Camera::setActive(bool active) {
  m_active = active;

  if (!active) {
    firstMouse = true;
  }
}

void Camera::move(float deltaTime) {
  float speedMultiplier = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 3.0 : 1.0f;

  const float cameraSpeed = speed * deltaTime * speedMultiplier;

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraUp;

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraUp;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::updateVectors(float deltaTime) {
  float mouseX, mouseY;

  {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseX = static_cast<float>(x);
    mouseY = static_cast<float>(y);
  }

  if (firstMouse) {
    lastX = mouseX;
    lastY = mouseY;
    firstMouse = false;
  }

  float xOffset = mouseX - lastX;
  float yOffset = lastY - mouseY;
  lastX = mouseX;
  lastY = mouseY;

  xOffset *= sensitivity * deltaTime;
  yOffset *= sensitivity * deltaTime;

  yaw += xOffset;
  pitch = std::clamp(pitch + yOffset, -89.0f, 89.0f);

  constexpr auto rad = glm::radians<double>;

  glm::vec3 front;
  front.x = static_cast<float>(cos(rad(yaw)) * cos(rad(pitch)));
  front.y = static_cast<float>(sin(rad(pitch)));
  front.z = static_cast<float>(sin(rad(yaw)) * cos(rad(pitch)));
  cameraFront = glm::normalize(front);
}
}  // namespace engine
