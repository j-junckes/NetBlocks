#include "Camera.hpp"

Camera::Camera(glm::vec3 startPosition, float startYaw, float startPitch, glm::vec3 startWorldUp) : front(
  glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(5.0f), mouseSensitivity(0.1f), zoom(60.0f) {
  position = startPosition;
  yaw = startYaw;
  pitch = startPitch;
  worldUp = startWorldUp;
  updateCameraVectors();
}

Camera::~Camera() = default;

void Camera::updateCameraVectors() {
  glm::vec3 newFront;
  newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  newFront.y = sin(glm::radians(pitch));
  newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(newFront);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

void Camera::processKeyboard(const std::shared_ptr<Input>& input, float deltaTime) {
  float velocity = movementSpeed * deltaTime;

  if (input->isKey(SDL_SCANCODE_W))
    position += front * velocity;
  if (input->isKey(SDL_SCANCODE_S))
    position -= front * velocity;
  if (input->isKey(SDL_SCANCODE_A))
    position -= right * velocity;
  if (input->isKey(SDL_SCANCODE_D))
    position += right * velocity;
}

void Camera::processMouseMovement(const std::shared_ptr<Input>& input, bool constrainPitch) {
  auto xOffset = (float)input->getMouseDeltaX();
  auto yOffset = (float)input->getMouseDeltaY();
  xOffset *= mouseSensitivity;
  yOffset *= mouseSensitivity;

  yaw += xOffset;
  pitch -= yOffset;

  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
  zoom -= yOffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 60.0f)
    zoom = 60.0f;
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(int width, int height) const {
  return glm::perspective(glm::radians(zoom), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
}
