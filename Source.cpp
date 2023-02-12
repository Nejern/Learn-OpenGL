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

// Глобальные переменные времени
long double gameTime = 0.f;
float timeScale = 1.0f;

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

  // Координаты вершин куба
  Vertex vertices[] = {
      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f),
      Vertex(0.5f, -0.5f, -0.5f, 1.0f, 0.0f),
      Vertex(0.5f, 0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(0.5f, 0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f),

      Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f),
      Vertex(0.5f, -0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 1.0f),
      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 1.0f),
      Vertex(-0.5f, 0.5f, 0.5f, 0.0f, 1.0f),
      Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f),

      Vertex(-0.5f, 0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(-0.5f, 0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f),
      Vertex(-0.5f, 0.5f, 0.5f, 1.0f, 0.0f),

      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(0.5f, 0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(0.5f, -0.5f, 0.5f, 0.0f, 0.0f),
      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f),

      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(0.5f, -0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(0.5f, -0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f),
      Vertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),

      Vertex(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f),
      Vertex(0.5f, 0.5f, -0.5f, 1.0f, 1.0f),
      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f),
      Vertex(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f),
      Vertex(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f),
  };

  // Позиции кубов
  glm::vec3 cubePos[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f),
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

  /* -----------------------[Камера]----------------------- */
  // Позиция камеры
  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  // Направление камеры
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
  // Вектор "право" камеры
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
  // Вектор "вверх" камеры
  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  const float radius = 10.f;
  float camX, camZ;

  /* -----------------------[Отрисовка]----------------------- */
  // Использование шейдерной программы
  glUseProgram(shader.Program);

  // Цвет очистки экрана
  glClearColor(29.f / 255, 32.f / 255, 33.f / 255, 1.f);

  // Привязка текстур
  glBindTextureUnit(0, texture_container.ID);
  glBindTextureUnit(1, texture_emoji.ID);

  // Передача текстурных юнитов в шейдер
  glUniform1i(glGetUniformLocation(shader.Program, "Texture_1"), 0);
  glUniform1i(glGetUniformLocation(shader.Program, "Texture_2"), 1);

  /* Матрицы */
  // Матрица модели
  glm::mat4 model = glm::mat4(1.f);

  // Матрица вида
  glm::mat4 view = glm::mat4(1.f);

  // Матрица проекции
  glm::mat4 projection = glm::mat4(1.f);
  projection = glm::perspective(glm::radians(45.f),
                                (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);

  // Объявление переменных для передачи в шейдер
  GLint modelLoc = glGetUniformLocation(shader.Program, "model");
  GLint viewLoc = glGetUniformLocation(shader.Program, "view");
  GLint projectionLoc = glGetUniformLocation(shader.Program, "projection");
  // Передача матриц в шейдер
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Переменные времени
  long double realTime = 0.f;
  long double lastUpdeteTime = 0.f;
  double deltaTime = 0.f;

  // Depth test
  glEnable(GL_DEPTH_TEST);

  // Привязка VAO
  glBindVertexArray(VAO[0]);

  while (!glfwWindowShouldClose(window)) {
    // Обновление времени
    lastUpdeteTime = gameTime;
    gameTime += ((glfwGetTime() - realTime) * timeScale);
    realTime = glfwGetTime();
    deltaTime = gameTime - lastUpdeteTime;

    // Проверка наличия событий
    glfwPollEvents();

    // Очистка буфера цвета
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Камера
    camX = sin(gameTime) * radius;
    camZ = cos(gameTime) * radius;
    view = glm::lookAt(glm::vec3(camX, 0.f, camZ), glm::vec3(0.f, 0.f, 0.f),
                       glm::vec3(0.f, 1.f, 0.f));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    /* Отрисовка */
    // Цикл кубов
    for (int i = 0; i < (int)sizeof(cubePos) / (int)sizeof(glm::vec3); i++) {
      // Матрица модели
      model = glm::mat4(1.f);
      // Перемещение по позициям
      model = glm::translate(model, cubePos[i]);
      // Поворот
      model = glm::rotate(model, glm::radians(20.f * (i + 1) * (float)gameTime),
                          glm::vec3(1.f, 0.3f, 0.5f));
      // Передача матрицы в шейдер
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      // Отрисовка
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Замена цветового буфера
    glfwSwapBuffers(window);

    // Проверка наличия ошибок
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::cout << "OpenGL error: " << error << std::endl;
    }
  }
  // Отвязка VAO
  glBindVertexArray(0);

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
bool l_flag;
void key_callback(GLFWwindow *window, int key, int scanCode, int action,
                  int mode) {
  // Когда пользователь нажимает ESC, мы устанавливаем свойство
  // WindowShouldClose в true, и приложение после этого закроется
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  // Polygones mode
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    if (l_flag == 0) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      l_flag = 1;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      l_flag = 0;
    }
  }

  // Time scale
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    timeScale += 1.f;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    timeScale -= 1.f;
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    timeScale -= 0.1f;
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    timeScale += 0.1f;
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    timeScale = 1.f;
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
