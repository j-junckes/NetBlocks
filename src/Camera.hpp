#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "Input.hpp"

const float YAW = 90.0f;
const float PITCH = 0.0f;
const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

class Camera {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;
  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  explicit Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f),
                  float startYaw = -90.0f, float startPitch = 0.0f,
                  glm::vec3 startWorldUp = glm::vec3(0.0f, 1.0f, 0.0f));

  ~Camera();

  [[nodiscard]] glm::mat4 getViewMatrix() const;

  [[nodiscard]] glm::mat4 getProjectionMatrix(int width, int height) const;

  void updateCameraVectors();

  void processKeyboard(const std::shared_ptr<Input>& input, float deltaTime);

  void processMouseMovement(const std::shared_ptr<Input>& input, bool constrainPitch = true);

  void processMouseScroll(float yOffset);
};
