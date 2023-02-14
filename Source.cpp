// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
// Остальные библиотеки
#include <cmath>
#include <iostream>
// Остальные заголовочные файлы
#include "Headers/Camera.h" // Класс камеры
#include "Headers/Shader.h" // Класс шейдера

// Прототипы функций
// -----------------
// Функция обработки ошибок GLFW
void glfwErrorCallback(int error, const char *description);
// Функция обработки изменения размеров окна
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// Функция обработки нажатия клавиш
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode);
// Функция обработки движения мыши
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// Функция обработки колеса мыши
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// Генерация текстуры
unsigned int genTexturePath(const char *path);
// Движение камеры
void doMovement();

// Константы
// ---------
const GLuint SCR_WIDTH = 1280, SCR_HEIGHT = 720;

// Переменные камеры
// -----------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// Управление камерой
bool w_flag = 0;
bool a_flag = 0;
bool s_flag = 0;
bool d_flag = 0;

// Глобальные переменные времени
// -----------------------------
long double realTime = 0.0f; // Реальное время с начала запуска программы
long double gameTime = 0.0f; // Игровое время
double deltaTime = 0.0f; // Игровое время между текущим кадром и предыдущим
long double lastFrame = 0.0f; // Игровое время последнего кадра
double timeScale = 1.0f; // Масштаб игрового времени

// Точка входа в программу
int main() {
  // Инициализация и конфигурация GLFW
  // ---------------------------------
  glfwInit(); // Инициализация GLFW
  // Настройка GLFW
  // Мажорная и минорная версии OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Мажорная версия OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Минорная версия OpenGL
  // Ядро профиля OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Отключение возможности изменения размеров окна
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Режим совместимости с MacOS
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Для Mac OS
#endif

  // Создание окна
  // -------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
  // Проверка на успешное создание окна
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  // Установка текущего контекста OpenGL
  glfwMakeContextCurrent(window);

  // Регистрация функций обратного вызова
  // -----------------------------------
  // Функция обработки ошибок GLFW
  glfwSetErrorCallback(glfwErrorCallback);
  // Функция обработки изменения размеров окна
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // Функция обработки нажатия клавиш
  glfwSetKeyCallback(window, key_callback);
  // Функция обработки движения мыши
  glfwSetCursorPosCallback(window, mouse_callback);
  // Функция обработки колеса мыши
  glfwSetScrollCallback(window, scroll_callback);

  // Настройка ввода мыши
  // --------------------
  // Установка курсора в центр окна
  glfwSetCursorPos(window, lastX, lastY);
  // Установка режима захвата курсора
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Инициализация GLEW
  // ------------------
  // Включение режима экспериментальных функций
  glewExperimental = GL_TRUE;
  // Инициализация GLEW
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // Определение области отрисовки
  // -----------------------------
  // Установка размеров области отрисовки
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  // Шейдеры
  // -------
  Shader lightShader("./Shaders/lightVertexShader.glsl",
                     "./Shaders/lightFragmentShader.glsl");
  Shader lampShader("./Shaders/lightVertexShader.glsl",
                    "./Shaders/lampFragmentShader.glsl");

  // Вершины
  // -------
  // Координаты вершин
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
      0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

      -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

      -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f,
      0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,
      0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};

  // Буфер вершин
  // ------------
  // Создание буфера вершин и массива вершин
  unsigned int VBO, lightVAO;
  glCreateVertexArrays(1, &lightVAO); // Создаем VAO
  glCreateBuffers(1, &VBO);           // Создаем VBO

  // Заполняем буфер вершин
  glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);

  // Включаем и устанавливаем атрибуты вершин
  // Позиция вершин
  glEnableVertexArrayAttrib(lightVAO, 0);
  glVertexArrayAttribBinding(lightVAO, 0, 0);
  glVertexArrayAttribFormat(lightVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

  // Связываем атрибуты с текущим VBO
  glVertexArrayVertexBuffer(lightVAO, 0, VBO, 0, 3 * sizeof(float));

  // Подготовка к рендерингу
  // -----------------------
  // Матрица модели
  glm::mat4 model = glm::mat4(1.0f);
  // Матрица вида
  glm::mat4 view = glm::mat4(1.0f);
  // Матрица проекции
  glm::mat4 projection = glm::mat4(1.0f);

  // Цвет очистки экрана
  glClearColor(29.f / 255, 32.f / 255, 33.f / 255, 1.f);
  // Включение теста глубины
  glEnable(GL_DEPTH_TEST);

  // Привязка VAO
  glBindVertexArray(lightVAO);

  // Привязка шейдера
  lightShader.use();
  // Установка цвета объекта
  lightShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
  // Установка цвета источника света
  lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

  // Цикл рендеринга
  // ---------------
  while (!glfwWindowShouldClose(window)) {
    // Обновление времени
    lastFrame = gameTime;
    if (timeScale <= 0) {
      timeScale = 0.000001;
    } else if (timeScale > 1) {
      timeScale = 1;
    }
    gameTime += (glfwGetTime() - realTime) * timeScale;
    realTime = glfwGetTime();
    deltaTime = gameTime - lastFrame;

    // Движение камеры
    doMovement();

    // Очистка буфера цвета и буфера глубины
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Обновление матрицы вида
    view = camera.GetViewMatrix();
    // Обновление матрицы проекции
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Отрисовка
    // ---------
    // Источник света
    // --------------
    // Привязка шейдера
    lampShader.use();
    // Матрица модели
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.2f, 1.0f, 2.0f));
    model = glm::scale(model, glm::vec3(0.2f));
    // Применение матрицы модели
    lampShader.setMat4("model", model);
    // Применение матрицы вида и проекции
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);
    // Отрисовка источника света
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Объекты
    // -------
    // Привязка шейдера
    lightShader.use();
    // Матрица модели
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    // Применение матрицы модели
    lightShader.setMat4("model", model);
    // Применение матрицы вида и проекции
    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    // Отрисовка объектов
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Проверка и вызов событий
    glfwPollEvents();

    // Замена буферов кадра
    glfwSwapBuffers(window);
  }
  // Отвязка VAO
  glBindVertexArray(0);

  // Очистка ресурсов
  // ----------------
  // Удаление шейдерной программы
  lightShader.deleteProgram();
  // Удаление VAO
  glDeleteVertexArrays(1, &lightVAO);
  // Удаление VBO
  glDeleteBuffers(1, &VBO);
  // Освобождение ресурсов GLFW
  glfwTerminate();
  return 0;
}

// Реализация функций
// ------------------
// Колбэк обработки ошибок
void glfwErrorCallback(int error, const char *description) {
  std::cerr << "Error: " << description << std::endl;
}
// Колбэк обработки ввода
bool l_flag = 0;
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  // Закрытие окна
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Управление камерой
  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    w_flag = 1;
  if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    w_flag = 0;
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    a_flag = 1;
  if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    a_flag = 0;
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    s_flag = 1;
  if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    s_flag = 0;
  if (key == GLFW_KEY_D && action == GLFW_PRESS)
    d_flag = 1;
  if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    d_flag = 0;

  // time scale
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    timeScale += 0.1;
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    timeScale -= 0.1;
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    timeScale += 0.01;
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    timeScale -= 0.01;
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (timeScale != 1) {
      timeScale = 1;
    } else {
      timeScale = 0;
    }
  }

  // Polygon mode
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    if (l_flag) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      l_flag = 0;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      l_flag = 1;
    }
  }
}
// Колбэк обработки движения мыши
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}
// Колбэк обработки скролла мыши
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(yoffset);
}
// Колбэк обработки изменения размера окна
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
// Движение камеры
void doMovement() {
  if (w_flag)
    camera.ProcessKeyboard(FORWARD, deltaTime / timeScale);
  if (s_flag)
    camera.ProcessKeyboard(BACKWARD, deltaTime / timeScale);
  if (a_flag)
    camera.ProcessKeyboard(LEFT, deltaTime / timeScale);
  if (d_flag)
    camera.ProcessKeyboard(RIGHT, deltaTime / timeScale);
}
// Генереция текстуры
unsigned int genTexturePath(const char *path) {
  unsigned int ID;
  // Создание и привязка имени текстуры
  glCreateTextures(GL_TEXTURE_2D, 1, &ID);

  // Загрузка изображения, создание текстуры и генерация мип-уровней
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    if (nrChannels == 3) {
      glTextureStorage2D(ID, 1, GL_RGB8, width, height);
      glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                          data);
      glGenerateTextureMipmap(ID);
    } else if (nrChannels == 4) {
      glTextureStorage2D(ID, 1, GL_RGBA8, width, height);
      glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                          data);
      glGenerateTextureMipmap(ID);
    }
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  return ID;
}
