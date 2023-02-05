// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <iostream>

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

  // Инициализация GLEW
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Определение размеров окна
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Цикл отрисовки
  while (!glfwWindowShouldClose(window)) {
    // Проверка наличия событий
    glfwPollEvents();
    // Замена цветового буфера
    glfwSwapBuffers(window);
  }

  // Освобождение ресурсов
  glfwTerminate();
  return 0;
}
