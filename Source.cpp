/* OpenGL 4.6.0 Application */
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// Shader.h
#include "Shader.h"
// Texture.h
#include "Texture.h"

#include <cmath>
#include <iostream>

// Прототипы функций
// Колбэк для обработки нажатия клавиш клавиатуры
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode);

// Текущее состояне
void context_status();

// Ширина и высота окна
const GLuint WIDTH = 800, HEIGHT = 600;

// Структура для хранения координат вершин
struct Vertex {
  GLfloat aPos[3];
  GLfloat aTexCoord[2];

  Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat s, GLfloat t) {
    aPos[0] = x;
    aPos[1] = y;
    aPos[2] = z;
    aTexCoord[0] = s;
    aTexCoord[1] = t;
  }
};

int main(void) {
  /* -----------------------[Инициализация]----------------------- */
  /* GLFW */
  // Инициализация GLFW
  glfwInit();
  // Настройка GLFW
  // Задается минимальная требуемая версия OpenGL
  // Мажорная
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  // Минорная
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  // Установка профайла для которого создается контекст
  // В данном случае используется Core Profile
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Выключение возможности изменения размера окна
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Создание окна
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // Установка текущего контекста OpenGL
  glfwMakeContextCurrent(window);

  /* Колбэки */
  // Нажатие клавиш клавиатуры
  glfwSetKeyCallback(window, key_callback);

  /* GLEW */
  // Инициализация GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Определение размеров окна
  GLint width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  /* -----------------------[Шейдеры]----------------------- */
  // Создание шейдеров
  Shader shader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");

  /* ----------------------[Текстуры]---------------------- */
  /* Текстура 1 */
  Texture texture_container("Textures/container.jpg");

  glTextureParameteri(texture_container.ID, GL_TEXTURE_WRAP_S,
                      GL_MIRRORED_REPEAT);
  glTextureParameteri(texture_container.ID, GL_TEXTURE_WRAP_T,
                      GL_MIRRORED_REPEAT);

  glTextureParameteri(texture_container.ID, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(texture_container.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* Текстура 2 */
  Texture texture_emoji("Textures/emoji.png");

  glTextureParameteri(texture_emoji.ID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTextureParameteri(texture_emoji.ID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  glTextureParameteri(texture_emoji.ID, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(texture_emoji.ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /* -----------------------[Фигуры]----------------------- */

  /* Прямоугольник */

  // Координаты вершин
  Vertex vertices[] = {
      // Правый верхний угол
      Vertex(0.5f, 0.5f, 0.0f, 1.0f, 1.0f),
      // Левый верхний угол
      Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
      // Правый нижний угол
      Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
      // Левый нижний угол
      Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
  };

  // Индексы вершин
  GLuint indices[] = {
      1, 3, 2, // Первый треугольник
      1, 0, 2  // Второй треугольник
  };

  /* -----------------------[Буферы]----------------------- */
  // Объявление VAO, VBO, EBO
  GLuint VAO[1], VBO[1], EBO[1];

  // Генерация буферов
  glCreateVertexArrays(sizeof(VAO) / sizeof(VAO[0]), VAO);
  glCreateBuffers(sizeof(VBO) / sizeof(VBO[0]), VBO);
  glCreateBuffers(sizeof(EBO) / sizeof(EBO[0]), EBO);

  // Заполнение буфера вершин данными
  glNamedBufferData(VBO[0], sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Заполнение буфера индексов данными
  glNamedBufferData(EBO[0], sizeof(indices), indices, GL_STATIC_DRAW);

  // Настройка VAO
  // Атрибут позиции - 0
  // Включение атрибута вершин
  glEnableVertexArrayAttrib(VAO[0], 0);
  // Установка связи атрибута 0 с буфером 0 указанного VAO
  glVertexArrayAttribBinding(VAO[0], 0, 0);

  glVertexArrayAttribFormat(VAO[0], 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, aPos));
  // Указание для конкретного VAO, что атрибут 0 имеет 3 штуки значений,
  // представленных в виде GL_FLOAT, нормализовывать эти значения не надо
  // (GL_FALSE), значения начинаются с offsetof(Vertex, position)

  // Атрибут текстурных координат - 1
  // Включение атрибута вершин
  glEnableVertexArrayAttrib(VAO[0], 1);
  // Установка связи атрибута 2 с буфером 2 указанного VAO
  glVertexArrayAttribBinding(VAO[0], 1, 1);

  glVertexArrayAttribFormat(VAO[0], 1, 2, GL_FLOAT, GL_FALSE,
                            offsetof(Vertex, aTexCoord));
  // Указание для конкретного VAO, что атрибут 2 имеет 2 штуки значений,
  // представленных в виде GL_FLOAT, нормализовывать эти значения не надо
  // (GL_FALSE), значения начинаются с offsetof(Vertex, texCoord)

  // Установка буфера вершин
  // Позиция вершин
  glVertexArrayVertexBuffer(VAO[0], 0, VBO[0], 0, sizeof(Vertex));
  // Указание для конкретного VAO, что точка привязки 0 имеет буфер вершин VBO,
  // смещение 0 и размер sizeof(Vertex)

  // Текстурные координаты вершин
  glVertexArrayVertexBuffer(VAO[0], 1, VBO[0], 0, sizeof(Vertex));

  // Установка буфера индексов
  // Указание для конкретного VAO, что буфер индексов EBO
  glVertexArrayElementBuffer(VAO[0], EBO[0]);

  /* -----------------------[Отрисовка]----------------------- */
  // Цвет очистки экрана
  glClearColor(29.f / 255, 32.f / 255, 33.f / 255, 1.f);

  // Использование шейдерной программы
  glUseProgram(shader.Program);

  // Привязка текстур
  glBindTextureUnit(0, texture_container.ID);
  glBindTextureUnit(1, texture_emoji.ID);

  // Передача текстурных юнитов в шейдер
  glUniform1i(glGetUniformLocation(shader.Program, "Texture_1"), 0);
  glUniform1i(glGetUniformLocation(shader.Program, "Texture_2"), 1);

  // Матрица модели
  glm::mat4 model = glm::mat4(1.f);
  model = glm::rotate(model, glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));
  // Матрица вида
  glm::mat4 view = glm::mat4(1.f);
  view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));
  // Матрица проекции
  glm::mat4 projection = glm::mat4(1.f);
  projection = glm::perspective(glm::radians(45.f),
                                (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);

  GLint modelLoc = glGetUniformLocation(shader.Program, "model");
  GLint viewLoc = glGetUniformLocation(shader.Program, "view");
  GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
  // Передача матриц в шейдер
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  GLfloat time = 0.f;
  GLfloat timeScale = 1.0f;
  GLfloat realTime = 0.f;
  GLfloat lastUpdeteTime = 0.f;
  GLfloat deltaTime = 0.f;
  while (!glfwWindowShouldClose(window)) {
    // Обновление времени
    lastUpdeteTime = time;
    time += (glfwGetTime() - realTime) * timeScale;
    realTime = glfwGetTime();
    deltaTime = time - lastUpdeteTime;

    // Проверка наличия событий
    glfwPollEvents();

    // Очистка буфера цвета
    glClear(GL_COLOR_BUFFER_BIT);

    // Изменение матрицы трансформации
    model = glm::rotate(model, glm::radians(30.f * deltaTime),
                        glm::vec3(0.f, 0.f, 1.f));

    /* Отрисовка */
    // Привязка VAO
    glBindVertexArray(VAO[0]);
    // Отрисовка примитивов
    // Первая фигура
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
                   GL_UNSIGNED_INT, 0);
    // Вторая фигура

    // Отвязка VAO
    glBindVertexArray(0);

    // Замена цветового буфера
    glfwSwapBuffers(window);

    // Проверка наличия ошибок
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::cout << "OpenGL error: " << error << std::endl;
    }
  }

  /* Очистка ресурсов */
  // Удаление текстур
  glDeleteTextures(1, &texture_container.ID);
  glDeleteTextures(1, &texture_emoji.ID);
  // Удаление VAO
  glDeleteVertexArrays(sizeof(VAO) / sizeof(VAO[0]), VAO);
  // Удаление VBO
  glDeleteBuffers(sizeof(VBO) / sizeof(VBO[0]), VBO);
  // Удаление EBO
  glDeleteBuffers(sizeof(EBO) / sizeof(EBO[0]), EBO);
  // Удаление шейдерной программы
  glDeleteProgram(shader.Program);
  // Освобождение ресурсов
  glfwTerminate();
  return 0;
}

/* Реализация функций */
// Колбэк для обработки нажатия клавиш клавиатуры
int space_flag = 0;
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode) {
  // Когда пользователь нажимает ESC, мы устанавливаем свойство
  // WindowShouldClose в true, и приложение после этого закроется
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (space_flag == 0) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      space_flag = 1;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      space_flag = 0;
    }
  }
}

// Текущее состояне
void context_status() {
  int status;
  // VAO
  std::cout << "-----------" << std::endl;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &status);
  std::cout << "Current VAO: " << status << std::endl;
  // VBO
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &status);
  std::cout << "Current VBO: " << status << std::endl;
  // EBO
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &status);
  std::cout << "Current EBO: " << status << std::endl;
  // Shader
  glGetIntegerv(GL_CURRENT_PROGRAM, &status);
  std::cout << "Current Shader: " << status << std::endl;
  // Texture
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &status);
  std::cout << "Current Texture: " << status << std::endl;
  std::cout << "-----------" << std::endl;
}
