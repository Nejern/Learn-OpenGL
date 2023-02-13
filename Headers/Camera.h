#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GL/gl.h>

// Объявление нескольких возможных значений для действий камеры
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Значения камеры по умолчанию
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;

// Класс камеры, который обрабатывает ввод и вычисляет матрицу вида
class Camera {
public:
  glm::vec3 Position; // Позиция камеры
  glm::vec3 Front;    // Вектор направления камеры
  glm::vec3 Up;       // Вектор верха камеры
  glm::vec3 Right;    // Вектор правой стороны камеры
  glm::vec3 WorldUp;  // Вектор верха мира

  // Эйлеровы углы камеры
  float Yaw; // Угол поворота вокруг оси Y (рыскание)
  float Pitch; // Угол поворота вокруг оси X (тангаж)

  // Камерные параметры
  float MovementSpeed; // Скорость перемещения камеры
  float MouseSensitivity; // Чувствительность мыши
  float Zoom;             // Уровень зума

  // Конструктор с векторами
  Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f),
         glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), float yaw = YAW,
         float pitch = PITCH)
      : Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }
  // Конструктор с скалярами
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch)
      : Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  // Функция возвращающая матрицу вида
  glm::mat4 GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
    // glm::lookAt определяет матрицу вида
    // Параметры: позиция камеры, точка, куда смотрит камера, вектор верха
  }

  // Функция обработки ввода
  void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
      Position += Front * velocity;
    if (direction == BACKWARD)
      Position -= Front * velocity;
    if (direction == LEFT)
      Position -= Right * velocity;
    if (direction == RIGHT)
      Position += Right * velocity;
  }

  // Функция обработки ввода мыши
  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Убеждаемся, что при тангаже не выходим за границы
    if (constrainPitch) {
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    // Обновляем векторы камеры
    updateCameraVectors();
  }

  // Функция обработки ввода колеса мыши
  void ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
      Zoom -= yoffset;
    if (Zoom <= 1.0f)
      Zoom = 1.0f;
    if (Zoom >= 45.0f)
      Zoom = 45.0f;
  }

private:
  // Функция обновления векторов камеры
  void updateCameraVectors() {
    // Вычисляем новые вектора Front, Right и Up
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif
