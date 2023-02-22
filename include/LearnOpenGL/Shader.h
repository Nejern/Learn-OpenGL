#ifndef SHADER_H
#define SHADER_H

// GLAD
#include <glad/gl.h>
// GLM
#include <glm/glm.hpp>

// Остальные библиотеки
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Класс шейдера
class Shader {
public:
  unsigned int ID; // ID шейдера
  // Конструктор считывает и строит шейдер
  // -------------------------------------
  Shader(const char *vertexPath, const char *fragmentPath) {
    // Получаем код вершинного шейдера из файла
    // Вершинный шейдер
    std::string vertexCode;
    std::ifstream vShaderFile;
    // Фрагментный шейдер
    std::string fragmentCode;
    std::ifstream fShaderFile;

    // Убеждаемся, что ifstream объекты могут выкинуть исключения
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // Пытаемся открыть файлы
    try {
      // Открываем файлы
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      // Создаем потоки для хранения данных
      std::stringstream vShaderStream, fShaderStream;
      // Читаем данные из файловых буферов в потоки
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // Закрываем файлы
      vShaderFile.close();
      fShaderFile.close();
      // Конвертируем потоки в строковые переменные
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    // Конвертируем строковые переменные в массивы символов
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Строим шейдер
    // -------------
    // Объявляем переменные для хранения ID вершинного и фрагментного шейдеров
    unsigned int vertex, fragment;

    // Создаем вершинный шейдер
    // Строим шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    // Передаем код шейдера
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    // Компилируем шейдер
    glCompileShader(vertex);
    // Проверяем на ошибки
    checkCompileErrors(vertex, "VERTEX");

    // Создаем фрагментный шейдер
    // Строим шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    // Передаем код шейдера
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    // Компилируем шейдер
    glCompileShader(fragment);
    // Проверяем на ошибки
    checkCompileErrors(fragment, "FRAGMENT");

    // Создаем шейдерную программу
    ID = glCreateProgram();
    // Прикрепляем вершинный шейдер
    glAttachShader(ID, vertex);
    // Прикрепляем фрагментный шейдер
    glAttachShader(ID, fragment);
    // Связываем шейдерную программу
    glLinkProgram(ID);
    // Проверяем на ошибки
    checkCompileErrors(ID, "PROGRAM");

    // Удаляем шейдеры, так как они уже связаны с
    // нашей программой и больше не нужны
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  // Активация шейдерной программы
  // ------------------------------------------------------------------------
  void use() { glUseProgram(ID); }

  // Удаление шейдерной программы
  // ------------------------------------------------------------------------
  void deleteProgram() { glDeleteProgram(ID); }

  // Утилитные функции
  // ------------------------------------------------------------------------
  void setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  // ------------------------------------------------------------------------
  void setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setUInt(const std::string &name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  // ------------------------------------------------------------------------
  void setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
  }
  // ------------------------------------------------------------------------
  void setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  // ------------------------------------------------------------------------
  void setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  }
  // ------------------------------------------------------------------------
  void setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  // ------------------------------------------------------------------------
  void setVec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
  }
  // ------------------------------------------------------------------------
  void setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
  }

private:
  // Проверка на ошибки компиляции/линковки шейдеров
  // -----------------------------------------------
  void checkCompileErrors(unsigned int shader, std::string type) {
    int success; // Переменная для хранения результата
    char infoLog[1024]; // Переменная для хранения лога ошибок
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout
            << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
            << infoLog
            << "\n -- --------------------------------------------------- -- "
            << std::endl;
      }
    }
  }
};
#endif
