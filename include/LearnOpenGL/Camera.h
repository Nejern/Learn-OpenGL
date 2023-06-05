#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glad/gl.h"

// Объявление нескольких возможных значений для действий камеры
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Значения камеры по умолчанию
const float YAW = -90.0f; // Угол поворота вокруг оси Y (Рыскание)
const float PITCH = 0.0f; // Угол поворота вокруг оси X (Тангаж)
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
  float MovementSpeed = SPEED; // Скорость перемещения камеры
  float MouseSensitivity = SENSITIVTY; // Чувствительность мыши
  float Zoom = ZOOM;                   // Уровень зума
  float MaxZoom = 45.f; // Максимальный уровень зума
  float MinZoom = 1.0f; // Минимальный уровень зума

  // Конструктор с векторами
  Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f),
         glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), float yaw = YAW,
         float pitch = PITCH) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }
  // Конструктор с скалярами
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  // Функция возвращающая матрицу вида
  glm::mat4 GetViewMatrix() {
    // Первое что нам нужно сделать это получить позицию камеры
    // Позиция = Position

    // Затем нам нужно получить вектор направления камеры
    // Вектор направления = Front

    // Затем нам нужно получить вектор верха камеры
    // Вектор верха = Up

    // Затем нам нужно получить вектор правой стороны камеры
    // Вектор правой стороны = Right

    // Затем нам нужно получить матрицу сдвига
    glm::mat4 translation = glm::mat4(1.0f);
    translation[3][0] = -Position.x;
    translation[3][1] = -Position.y;
    translation[3][2] = -Position.z;

    // Затем нам нужно получить матрицу поворота
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = Right.x;
    rotation[1][0] = Right.y;
    rotation[2][0] = Right.z;
    rotation[0][1] = Up.x;
    rotation[1][1] = Up.y;
    rotation[2][1] = Up.z;
    rotation[0][2] = -Front.x;
    rotation[1][2] = -Front.y;
    rotation[2][2] = -Front.z;

    // Затем нам нужно перемножить матрицу поворота и матрицу сдвига
    // Матрица вида = rotation * translation
    return rotation * translation;
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
    if (Zoom >= MinZoom && Zoom <= MaxZoom)
      Zoom -= yoffset;
    if (Zoom <= MinZoom)
      Zoom = MinZoom;
    if (Zoom >= MaxZoom)
      Zoom = MaxZoom;
  }

private:
  // Функция обновления векторов камеры
  void updateCameraVectors() {
    // Вычисляем новые вектора Front, Right и Up
    glm::vec3 front;
    front.x = (float)(cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
    front.y = (float)(sin(glm::radians(Pitch)));
    front.z = (float)(sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif
