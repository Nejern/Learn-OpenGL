// GLAD
#include <glad/gl.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
// imgui
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
// Остальные библиотеки
#include <cmath>
#include <iostream>
// Остальные заголовочные файлы
#include "../include/LearnOpenGL/Camera.h" // Класс камеры
#include "../include/LearnOpenGL/Shader.h" // Класс шейдера

// Прототипы функций колбэков
// --------------------------

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

// Прототипы остальных функций
// ---------------------------

// Генерация текстуры
unsigned int genTexturePath(const char *path);

// Движение камеры
void doMovement();

// Проверка коэффициента времени
void checkTimeScale();

// Константы размера окна
// -----------------------
const GLuint SCR_WIDTH = 1280, SCR_HEIGHT = 720;

// Переменные камеры
// -----------------
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Камера
GLfloat lastX = SCR_WIDTH / 2.0f; // Последняя позиция мыши по оси X
GLfloat lastY = SCR_HEIGHT / 2.0f; // Последняя позиция мыши по оси Y
bool firstMouse = true; // Первое движение мыши
bool inputFlag = 1;
/* Управление камерой */
bool w_flag = 0;
bool a_flag = 0;
bool s_flag = 0;
bool d_flag = 0;

// Глобальные переменные времени
// -----------------------------
long double realTime = 0.0f; // Реальное время с начала запуска программы
long double gameTime = 0.0f; // Игровое время
long double deltaTime = 0.0f; // Игровое время между текущим кадром и предыдущим
long double lastFrame = 0.0f; // Игровое время последнего кадра
long double timeScale = 1.0f; // Масштаб игрового времени
double timeScaleMax = 1.0f; // Максимальный масштаб игрового времени
double timeScaleMin = 0.0000001f; // Минимальный масштаб игрового времени

// Флаг перемещения источника света
// --------------------------------
bool lampMoveFlag = 0;

// Точка входа в программу
int main() {
  // Инициализация и конфигурация GLFW
  // ---------------------------------
  glfwInit();

  /* Настройка GLFW */
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

  // GLAD
  // ----
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::cout << "Failed to initialize OpenGL context\n" << std::endl;
    return -1;
  }

  // Определение области отрисовки
  // -----------------------------
  // Установка размеров области отрисовки
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  // Шейдеры
  // -------
  // Шейдер для отрисовки куба
  Shader objShader("./Shaders/lightVertexShader.glsl",
                   "./Shaders/lightFragmentShader.glsl");
  // Шейдер для отрисовки источника света
  Shader lampShader("./Shaders/lampVertexShader.glsl",
                    "./Shaders/lampFragmentShader.glsl");

  // Вершины
  // -------
  // Координаты вершин
  float vertices[] = {
      // Дальняя грань
      // Первый треугольник
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Левый нижний угол
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // Правый нижний угол
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // Правый верхний угол
      // Второй треугольник
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // Правый верхний угол
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // Левый верхний угол
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Левый нижний угол

      // Ближняя грань
      // Первый треугольник
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Левый нижний угол
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // Правый нижний угол
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Правый верхний угол
      // Второй треугольник
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Правый верхний угол
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // Левый верхний угол
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Левый нижний угол

      // Левая грань
      // Первый треугольник
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Левый верхний угол
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      // Второй треугольник
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Правый нижний угол
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол

      // Правая грань
      // Первый треугольник
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Левый верхний угол
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      // Второй треугольник
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Правый нижний угол
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол

      // Нижняя грань
      // Первый треугольник
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // Правый нижний угол
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      // Второй треугольник
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Левый верхний угол
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол

      // Верхняя грань
      // Первый треугольник
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Левый нижний угол
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // Правый нижний угол
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      // Второй треугольник
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Правый верхний угол
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // Левый верхний угол
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // Левый нижний угол
  };

  // Объекты
  glm::vec3 cubePos[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f),
  };

  // Источник света
  // --------------
  glm::vec3 lampPos(1.5f, 1.5f, 1.5f);
  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

  // Буфер вершин для куба
  // ---------------------
  unsigned int cubeVBO;
  glCreateBuffers(1, &cubeVBO);
  // Заполняем буфер вершин
  glNamedBufferStorage(cubeVBO, sizeof(vertices), vertices, 0);

  // Буфер массива вершин для объекта
  // --------------------------------
  /* Создаем VAO */
  unsigned int objVAO;
  glCreateVertexArrays(1, &objVAO); // Создаем VAO

  /* Включаем и устанавливаем атрибуты вершин */
  // Позиция вершин
  glVertexArrayAttribFormat(objVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(objVAO, 0, 0);
  glEnableVertexArrayAttrib(objVAO, 0);
  // Нормали
  glVertexArrayAttribFormat(objVAO, 1, 3, GL_FLOAT, GL_FALSE,
                            3 * sizeof(float));
  glVertexArrayAttribBinding(objVAO, 1, 0);
  glEnableVertexArrayAttrib(objVAO, 1);
  // Текстурные координаты
  glVertexArrayAttribFormat(objVAO, 2, 2, GL_FLOAT, GL_FALSE,
                            6 * sizeof(float));
  glVertexArrayAttribBinding(objVAO, 2, 0);
  glEnableVertexArrayAttrib(objVAO, 2);
  // Связываем атрибуты с текущим VBO
  glVertexArrayVertexBuffer(objVAO, 0, cubeVBO, 0, 8 * sizeof(float));

  // Буфер массива вершин для источника света
  // ---------------------------------
  // Создаем VAO
  unsigned int lampVAO;
  glCreateVertexArrays(1, &lampVAO);

  // Включаем и устанавливаем атрибуты вершин
  // Позиция вершин
  glVertexArrayAttribFormat(lampVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(lampVAO, 0, 0);
  glEnableVertexArrayAttrib(lampVAO, 0);
  // Связываем атрибуты с текущим VBO
  glVertexArrayVertexBuffer(lampVAO, 0, cubeVBO, 0, 8 * sizeof(float));

  // Текстуры
  // --------
  /* Контейнер */
  unsigned int containerTexture = genTexturePath("./Textures/container2.png");
  // Настойка
  glTextureParameteri(containerTexture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTextureParameteri(containerTexture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTextureParameteri(containerTexture, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(containerTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Привязка
  glBindTextureUnit(0, containerTexture);

  /* Металическая обводка контейнера */
  unsigned int containerBorderTexture =
      genTexturePath("./Textures/container2_specular.png");
  // Настойка
  glTextureParameteri(containerBorderTexture, GL_TEXTURE_WRAP_S,
                      GL_MIRRORED_REPEAT);
  glTextureParameteri(containerBorderTexture, GL_TEXTURE_WRAP_T,
                      GL_MIRRORED_REPEAT);
  glTextureParameteri(containerBorderTexture, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(containerBorderTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Привязка
  glBindTextureUnit(1, containerBorderTexture);

  // Настройка imgui
  // ---------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  // Настройка камеры
  // ----------------
  camera.MinZoom = 0.5f;  // Минимальное значение зума
  camera.MaxZoom = 45.0f; // Максимальное значение зума
  camera.Zoom = camera.MaxZoom; // Зум по умолчанию

  // Матрицы
  // -------
  // Матрица модели
  glm::mat4 model = glm::mat4(1.0f);
  // Матрица вида
  glm::mat4 view = glm::mat4(1.0f);
  // Матрица проекции
  glm::mat4 projection = glm::mat4(1.0f);

  // Подготовка к рендерингу
  // -----------------------
  // Цвет очистки экрана
  glClearColor(29.f / 255, 32.f / 255, 33.f / 255, 1.f);
  // Включение теста глубины
  glEnable(GL_DEPTH_TEST);

  /* Передача данных в шейдеры */

  // Шейдер объекта
  // Привязка шейдера
  objShader.use();
  // Установка текстурных юнитов
  // Установка материала
  objShader.setInt("material.diffuse", 0);
  objShader.setInt("material.specular", 1);
  objShader.setFloat("material.shininess", 64.0f);

  // Шейдер источника света
  // Привязка шейдера
  lampShader.use();
  // Установка цвета источника света
  lampShader.setVec3("lightColor", lightColor);

  // Цикл рендеринга
  // ---------------
  while (!glfwWindowShouldClose(window)) {
    // Обновление времени
    // ------------------
    lastFrame = gameTime; // Запоминаем игровое время предыдущего кадра
    gameTime +=
        (glfwGetTime() - realTime) * timeScale; // Обновляем игровое время
    realTime = glfwGetTime(); // Запоминаем реальное время
    deltaTime = gameTime - lastFrame; // Вычисляем время между кадрами

    // Новый кадр ImGui
    // ----------------
    if (!inputFlag) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }

    // Обновление камеры
    // -----------------
    // Движение камеры
    if (inputFlag)
      doMovement();
    // Матрица вида
    view = camera.GetViewMatrix();
    // Матрица проекции
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // Обработка логики
    // ----------------
    /* Источник света */
    // Перемещение источника света
    if (lampMoveFlag) {
      lampPos = camera.Position + glm::vec3(1.f) * camera.Front;
    }
    // Параметры источника света
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    glm::vec3 specularColor = lightColor * glm::vec3(1.f);

    // Отрисовка
    // ---------
    // Очистка буфера цвета и буфера глубины
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Источник света */
    // Привязка шейдера
    lampShader.use();
    // Прикрепление VAO
    glBindVertexArray(lampVAO);

    // Матрица модели
    model = glm::mat4(1.0f);
    model = glm::translate(model, lampPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lampShader.setMat4("model", model);
    // Матрица вида
    lampShader.setMat4("view", view);
    // Матрица проекции
    lampShader.setMat4("projection", projection);
    // Применение цвета источника света
    lampShader.setVec3("lightColor", lightColor);

    // Отрисовка примитивов
    glDrawArrays(GL_TRIANGLES, 0, 36);

    /* Объект */
    // Привязка шейдера
    objShader.use();
    // Прикрепление VAO
    glBindVertexArray(objVAO);

    // Матрица вида
    objShader.setMat4("view", view);
    // Матрица проекции
    objShader.setMat4("projection", projection);

    // Применение позиции источника света
    objShader.setVec3("lightPos", lampPos);
    objShader.setVec3("light.ambient", ambientColor);
    objShader.setVec3("light.diffuse", diffuseColor);
    objShader.setVec3("light.specular", specularColor);

    for (int i = 0; i < (int)(sizeof(cubePos) / sizeof(cubePos[0])); i++) {
      // Матрица модели
      model = glm::mat4(1.0f);
      model = glm::translate(model, cubePos[i]);
      model = glm::rotate(model, glm::radians(20.f * (float)(i + 1) * (float)gameTime),
                          glm::vec3(1.f, 0.3f, 0.5f));
      objShader.setMat4("model", model);

      // Применение матрицы нормали
      objShader.setMat3("normalMatrix",
                        glm::transpose(glm::inverse(view * model)));

      // Отрисовка объектов
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Окно ImGui
    // ----------
    if (!inputFlag) {
      /* Настройки окна */
      ImGui::Begin("ImGui Window");
      ImGui::ColorEdit3("Light color", &lightColor.x);
      ImGui::Text("Game Time: %Lf", gameTime);
      ImGui::Text("Time scale: %Lf", timeScale);
      ImGui::End();

      /* Отрисовка окна */
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Проверка и вызов событий
    // ------------------------
    // Проверка колбэков
    glfwPollEvents();

    // Замена буферов кадра
    // --------------------
    glfwSwapBuffers(window);
  }

  // Очистка ресурсов и завершение работы
  // ------------------------------------
  // ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  // Удаление VAO
  glDeleteVertexArrays(1, &objVAO);
  glDeleteVertexArrays(1, &lampVAO);
  // Удаление VBO
  glDeleteBuffers(1, &cubeVBO);
  // Освобождение ресурсов GLFW
  glfwTerminate();

  // Завершение программы
  return 0;
}

// Реализация функций колбэков
// ---------------------------
// Колбэк обработки ошибок
void glfwErrorCallback(int error, const char *description) {
  std::cerr << "Error: " << description << std::endl;
}

// Колбэк обработки ввода
bool l_flag = 0; // Флаг нажатия кнопки L
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

  // Игнорирование ввода
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    if (inputFlag) {
      inputFlag = 0;
      firstMouse = 1;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      inputFlag = 1;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
  }

  // Перемещение источника света
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    if (lampMoveFlag) {
      lampMoveFlag = 0;
    } else {
      lampMoveFlag = 1;
    }
  }

  // time scale
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    timeScale += 0.1;
    checkTimeScale();
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    timeScale -= 0.1;
    checkTimeScale();
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    timeScale += 0.01;
    checkTimeScale();
  }
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    timeScale -= 0.01;
    checkTimeScale();
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    if (timeScale != timeScaleMin) {
      timeScale = timeScaleMin;
    } else {
      timeScale = 1;
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
  if (inputFlag) {
    float xPos = (float)xpos;
    float yPos = (float)ypos;
    if (firstMouse) {
      lastX = xPos;
      lastY = yPos;
      firstMouse = false;
    }

    float xoffset = (xPos - lastX) * (camera.Zoom / 45.0f);
    float yoffset = (lastY - yPos) * (camera.Zoom / 45.0f);
    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xoffset, yoffset);
  }
}

// Колбэк обработки скролла мыши
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll((float)yoffset);
}

// Колбэк обработки изменения размера окна
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Остальные функции
// -----------------
// Движение камеры
void doMovement() {
  if (w_flag)
    camera.ProcessKeyboard(FORWARD, (float)(deltaTime / timeScale));
  if (s_flag)
    camera.ProcessKeyboard(BACKWARD, (float)(deltaTime / timeScale));
  if (a_flag)
    camera.ProcessKeyboard(LEFT, (float)(deltaTime / timeScale));
  if (d_flag)
    camera.ProcessKeyboard(RIGHT, (float)(deltaTime / timeScale));
}

// Проверка коэффициента времени
void checkTimeScale() {
  if (timeScale <= 0) {
    timeScale = timeScaleMin;
  } else if (timeScale > timeScaleMax) {
    timeScale = timeScaleMax;
  }
}

// Генереция текстуры
unsigned int genTexturePath(const char *path) {
  unsigned int ID;
  // Создание и привязка имени текстуры
  glCreateTextures(GL_TEXTURE_2D, 1, &ID);

  // Настройка чтения текстуры
  stbi_set_flip_vertically_on_load(true);

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
