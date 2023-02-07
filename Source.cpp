/* OpenGL 4.6.0 Application */
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// Shader.h
#include "Shader.h"

#include <cmath>
#include <iostream>

// Прототипы функций
// Колбэк для обработки нажатия клавиш клавиатуры
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode);

// Ширина и высота окна
const GLuint WIDTH = 800, HEIGHT = 600;

int main(void) {
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

  /* Шейдеры */
  // Создание шейдеров
  Shader shader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");

  // Вершины треугольника
  // Позиции вершин
  GLfloat vertices[] = {
      // Позиция          // Цвет
      0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // Верхний угол
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Нижний правый угол
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Нижний левый угол
  };

  /* VBO & VAO */
  // Объявление идентификаторов VBO и VAO
  GLuint VBO, VAO;
  // Генерация буферов
  glCreateBuffers(1, &VBO);
  glCreateVertexArrays(1, &VAO);
  // Привязка VBO к VAO
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 6 * sizeof(GLfloat));
  // Включение атрибутов вершин
  glEnableVertexArrayAttrib(VAO, 0); // Позиция
  glEnableVertexArrayAttrib(VAO, 1); // Цвет
  // Указание атрибутов вершин
  glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0); // Позиция
  glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE,
                            3 * sizeof(GLfloat)); // Цвет
  // Привязка атрибутов вершин
  glVertexArrayAttribBinding(VAO, 0, 0); // Позиция
  glVertexArrayAttribBinding(VAO, 1, 0); // Цвет
  // Заполнение буфера вершин
  glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

  /* Цикл отрисовки */

  // Цвет очистки экрана
  glClearColor(29.f / 255, 32.f / 255, 33.f / 255, 1.f);

  while (!glfwWindowShouldClose(window)) {
    // Проверка наличия событий
    glfwPollEvents();

    // Очистка буфера цвета
    glClear(GL_COLOR_BUFFER_BIT);

    /* Отрисовка */
    // Использование шейдерной программы
    shader.Use();
    // Привязка VAO
    glBindVertexArray(VAO);
    // Отрисовка примитивов
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
  // Удаление VAO
  glDeleteVertexArrays(1, &VAO);
  // Удаление VBO
  glDeleteBuffers(1, &VBO);
  // Удаление шейдерной программы
  glDeleteProgram(shader.Program);
  // Освобождение ресурсов
  glfwTerminate();
  return 0;
}

// Реализация функций
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
