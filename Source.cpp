// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <iostream>

// Прототипы функций
// Колбэк для обработки нажатия клавиш клавиатуры
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode);

int main(void) {
  // Инициализация GLFW
  glfwInit();
  // Настройка GLFW
  // Задается минимальная требуемая версия OpenGL
  // Мажорная
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // Минорная
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Установка профайла для которого создается контекст
  // В данном случае используется Core Profile
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Выключение возможности изменения размера окна
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Создание окна
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Learn OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Регистрация колбэков
  // Нажатие клавиш клавиатуры
  glfwSetKeyCallback(window, key_callback);

  // Установка текущего контекста OpenGL
  glfwMakeContextCurrent(window);
  // Инициализация GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Определение размеров окна
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Цвет очистки экрана
  float red, green, blue, alpha;
  red = 49.f / 255.f;
  green = 0.f;
  blue = 98.f / 255.f;
  alpha = 1.f;

  // Цикл отрисовки
  while (!glfwWindowShouldClose(window)) {
    // Проверка наличия событий
    glfwPollEvents();

    // Очистка буфера цвета
    glClearColor(red, green, blue, alpha);
    glClear(GL_COLOR_BUFFER_BIT);

    // Замена цветового буфера
    glfwSwapBuffers(window);

    // Проверка наличия ошибок
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::cout << "OpenGL error: " << error << std::endl;
    }
  }

  // Освобождение ресурсов
  glfwTerminate();
  return 0;
}

// Реализация функций
// Колбэк для обработки нажатия клавиш клавиатуры
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode) {
  // Когда пользователь нажимает ESC, мы устанавливаем свойство
  // WindowShouldClose в true, и приложение после этого закроется
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}
