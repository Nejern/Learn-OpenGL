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
  Shader ourShader("./Shaders/vertexShader.glsl",
                   "./Shaders/fragmentShader.glsl");

  // Вершины
  // -------
  // Координаты вершин
  float vertices[] = {
      // Позиции           // Координаты текстур
      // Дальняя грань
      // Первый треугольник
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Левый нижний угол
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // Правый нижний угол
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // Правый верхний угол
      // Второй треугольник
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // Правый верхний угол
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // Левый верхний угол
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Левый нижний угол

      // Передняя грань
      // Первый треугольник
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // Левый нижний угол
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // Правый нижний угол
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // Правый верхний угол
      // Второй треугольник
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // Правый верхний угол
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // Левый верхний угол
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // Левый нижний угол

      // Левая грань
      // Первый треугольник
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый верхний угол
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // Правый нижний угол
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый нижний угол
      // Второй треугольник
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый нижний угол
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // Левый верхний угол
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый верхний угол

      // Правая грань
      // Первый треугольник
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый верхний угол
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // Правый нижний угол
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый нижний угол
      // Второй треугольник
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый нижний угол
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // Левый верхний угол
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый верхний угол

      // Нижняя грань
      // Первый треугольник
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый верхний угол
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // Правый верхний угол
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // Правый нижний угол
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // Правый нижний угол
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // Левый нижний угол
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Левый верхний угол

      // Верхняя грань
      // Первый треугольник
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // Левый верхний угол
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // Правый верхний угол
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый нижний угол
      // Второй треугольник
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // Правый нижний угол
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,  // Левый нижний угол
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // Левый верхний угол
  };

  // Координаты кубов
  glm::vec3 cubePos[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  // Буфер вершин
  // ------------
  // Создание буфера вершин и массива вершин
  unsigned int VBO, VAO;
  glCreateVertexArrays(1, &VAO); // Создаем VAO
  glCreateBuffers(1, &VBO);      // Создаем VBO

  // Заполняем буфер вершин
  glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);

  // Включаем и устанавливаем атрибуты вершин
  // Позиция вершин
  glEnableVertexArrayAttrib(VAO, 0);
  glVertexArrayAttribBinding(VAO, 0, 0);
  glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  // Текстурные координаты
  glEnableVertexArrayAttrib(VAO, 1);
  glVertexArrayAttribBinding(VAO, 1, 0);
  glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

  // Связываем атрибуты с текущим VBO
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(float));

  // Загружаем и создаем текстуры
  // ----------------------------
  // Текстура 1
  // ----------
  // Чтение изображения и создание текстуры
  unsigned int texture_container = genTexturePath("./Textures/container.jpg");

  // Настройка текстуры
  // Установка повторения текстуры по оси S
  glTextureParameteri(texture_container, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  // Установка повторения текстуры по оси T
  glTextureParameteri(texture_container, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // Установка фильтра минимизации
  glTextureParameteri(texture_container, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  // Установка фильтра магнификации
  glTextureParameteri(texture_container, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Текстура 2
  // ----------
  // Чтение изображения и создание текстуры
  unsigned int texture_emoji = genTexturePath("./Textures/emoji.png");
  // Настройка текстуры
  // Установка повторения текстуры по оси S
  glTextureParameteri(texture_emoji, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  // Установка повторения текстуры по оси T
  glTextureParameteri(texture_emoji, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // Установка фильтра минимизации
  glTextureParameteri(texture_emoji, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  // Установка фильтра магнификации
  glTextureParameteri(texture_emoji, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Подготовка к рендерингу
  // -----------------------
  // Прменяем шейдерную программу
  ourShader.use();

  // Привязываем текстуры к текстурным блокам
  glBindTextureUnit(0, texture_container);
  glBindTextureUnit(1, texture_emoji);

  // Устанавливаем текстуры в шейдер
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);

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
  glBindVertexArray(VAO);

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
    if (w_flag) {
      camera.ProcessKeyboard(FORWARD, deltaTime / timeScale);
    }
    if (s_flag) {
      camera.ProcessKeyboard(BACKWARD, deltaTime / timeScale);
    }
    if (a_flag) {
      camera.ProcessKeyboard(LEFT, deltaTime / timeScale);
    }
    if (d_flag) {
      camera.ProcessKeyboard(RIGHT, deltaTime / timeScale);
    }

    // Очистка буфера цвета и буфера глубины
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Обновление матрицы вида
    view = camera.GetViewMatrix();
    // Обновление матрицы проекции
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Применение матрицы вида и проекции
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);

    // Отрисовка
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
      ourShader.setMat4("model", model);

      // Отрисовка примитивов
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Проверка и вызов событий
    glfwPollEvents();

    // Замена буферов кадра
    glfwSwapBuffers(window);
  }
  // Отвязка VAO
  glBindVertexArray(0);

  // Очистка ресурсов
  // ----------------
  // Удаление текстур
  glDeleteTextures(1, &texture_container);
  glDeleteTextures(1, &texture_emoji);
  // Удаление шейдерной программы
  ourShader.deleteProgram();
  // Удаление VAO
  glDeleteVertexArrays(1, &VAO);
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
