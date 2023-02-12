#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h> // Подключаем GLEW для получения всех необходимых заголовочных файлов OpenGL

class Shader {
public:
  // Идентификатор программы
  GLuint Program;
  // Конструктор считывает и собирает шейдер
  Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    /* Получаем исходный код шейдера из filepath */
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Удостоверимся, что ifstream объекты могут выкидывать исключения
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    try {
      // Открываем файлы
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // Читаем файлы в потоки
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // Закрываем файлы
      vShaderFile.close();
      fShaderFile.close();
      // Конвертируем потоки в GLchar массивы
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();

    /* Собираем шейдеры */
    GLint success;
    GLchar infoLog[512];

    // Вершинный шейдер
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Проверяем на ошибки
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // Фрагментный шейдер
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Проверяем на ошибки
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // Шейдерная программа
    this->Program = glCreateProgram();
    // Связываем шейдеры
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    // Линкуем шейдеры
    glLinkProgram(this->Program);
    // Проверяем на ошибки
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }

    // Удаляем шейдеры, так как они уже связаны с нашей программой и больше не нужны
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }
  // Использование программы
  void Use() { glUseProgram(this->Program); }
};

#endif
