// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

// Прототипы функций
// Колбэк для обработки нажатия клавиш клавиатуры
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode);

// Ширина и высота окна
const GLuint WIDTH = 800, HEIGHT = 600;

// Шейдеры
// Вершинный шейдер
const GLchar *vertexShaderSource = "#version 460 core\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "layout (location = 1) in vec3 color;\n"
                                   "out vec3 ourColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(position, 1.0);\n"
                                   "ourColor = color;\n"
                                   "}\n\0";
// Фрагментный шейдер
const GLchar *fragmentShaderSource = "#version 460 core\n"
                                     "in vec3 ourColor;\n"
                                     "out vec4 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = vec4(ourColor, 1.0f);\n"
                                     "}\n\0";

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
  /* Вершинный шейдер */
  // Создание шейдера
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Передача исходного кода шейдера
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  // Компиляция шейдера
  glCompileShader(vertexShader);
  // Проверка на ошибки компиляции
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  /* Фрагментный шейдер */
  // Создание шейдера
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  // Передача исходного кода шейдера
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  // Компиляция шейдера
  glCompileShader(fragmentShader);
  // Проверка на ошибки компиляции
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  /* Шейдерная программа */
  // Создание шейдерной программы
  GLuint shaderProgram = glCreateProgram();
  // Прикрепление шейдеров к шейдерной программе
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  // Связывание шейдеров в шейдерную программу
  glLinkProgram(shaderProgram);
  // Проверка на ошибки связывания
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  // Удаление шейдеров
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Вершины прямоугольника
  // Позиции вершин
  GLfloat vertices[] = {
      // Прямоугольник    // Цвет
      0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // Верхний угол
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Нижний правый угол
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Нижний левый угол
  };

  /* VBO & VAO */

  // Создание VBO
  GLuint VBO;
  glGenBuffers(1, &VBO);
  // Создание VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);

  // Привязка VAO
  glBindVertexArray(VAO);
  // Привязка VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Копирование массива вершин в буфер
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  // Указание атрибутов вершин
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (GLvoid *)0);
  // Указание атрибутов цвета
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (GLvoid *)(3 * sizeof(GLfloat)));
  // Включение вершинных атрибутов
  glEnableVertexAttribArray(0);
  // Включение атрибутов цвета
  glEnableVertexAttribArray(1);

  // Отвязка VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Отвязка VAO
  glBindVertexArray(0);

  // Цвет очистки экрана
  glClearColor(29.f/255, 32.f/255, 33.f/255, 1.f);

  /* Цикл отрисовки */
  while (!glfwWindowShouldClose(window)) {
    // Проверка наличия событий
    glfwPollEvents();

    // Очистка буфера цвета
    glClear(GL_COLOR_BUFFER_BIT);

    /* Отрисовка */
    // Использование шейдерной программы
    glUseProgram(shaderProgram);
    // Прямоугольник
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
  glDeleteProgram(shaderProgram);
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
