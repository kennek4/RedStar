#ifndef CAMERA_H
#define CAMERA_H

#include "classes/entity.hpp"
#include <GL/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum MOVEMENT_DIRECTION { FORWARD, BACKWARD, LEFT, RIGHT };

const glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 DEFAULT_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float DEFAULT_MOVE_SPEED = 2.5f;
const float DEFAULT_CAMERA_FOV = 90.0f;
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_CAMERA_SENSITIVITY = 0.1f;

class Camera : public Entity {
public:
  // Camera attributes
  // Inherits Position from Entity
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  // Euler angles
  float Yaw;
  float Pitch;

  // Camera options
  float MoveSpeed;
  float CameraSensitivity;
  float FOV;

  // Constructors
  Camera(glm::vec3 position = DEFAULT_POSITION,
         glm::vec3 worldUp = DEFAULT_WORLD_UP, float yaw = DEFAULT_YAW,
         float pitch = DEFAULT_PITCH)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MoveSpeed(DEFAULT_MOVE_SPEED),
        CameraSensitivity(DEFAULT_CAMERA_SENSITIVITY), FOV(DEFAULT_CAMERA_FOV) {
    Position = position;
    WorldUp = worldUp;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  Camera(glm::vec3 position, float moveSpeed, float cameraSensitivity) {
    Position = position;
    MoveSpeed = moveSpeed;
    CameraSensitivity = cameraSensitivity;
  }

  // Deconstructor

  void Move(MOVEMENT_DIRECTION direction, float deltaTime) {
    float velocity = MoveSpeed * deltaTime;
    switch (direction) {
    case FORWARD:
      Position += Front * velocity;
      break;
    case BACKWARD:
      Position -= Front * velocity;
      break;
    case RIGHT:
      Position += Right * velocity;
      break;
    case LEFT:
      Position -= Right * velocity;
      break;
    }
    Position.y = 0.0f;
    updateCameraVectors();
  }

  void ProcessMouseMovement(float xOffset, float yOffset,
                            GLboolean constrainPitch = true) {
    xOffset *= CameraSensitivity;
    yOffset *= CameraSensitivity;

    Yaw += xOffset;
    Pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
  }

  glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
  }

private:
  void updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
        Front, WorldUp)); // normalize the vectors, because their length gets
                          // closer to 0 the more you look up or down which
                          // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif // !CAMERA_H
