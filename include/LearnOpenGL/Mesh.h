#ifndef MESH_H
#define MESH_H

// GLAD
#include "glad/gl.h"
// GLM
#include <glm/glm.hpp>

// Остальные библиотеки
#include <string>
#include <vector>

// Остальные заголовочные файлы
#include "Shader.h" // Класс шейдера

#define MAX_BONE_INFLUENCE 4

// Структуры данных
// ----------------
/* Вершина */
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 Bitangent;
  // bone indexes which will influence this vertex
  int m_BoneIDs[MAX_BONE_INFLUENCE];
  // weights from each bone
  float m_Weights[MAX_BONE_INFLUENCE];
};

/* Текстура */
struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

// Класс Mesh
// ----------
class Mesh {
public:
  // Данные
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  float matShininess;
  unsigned int VAO;

  // Конструктор
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures, float matShininess) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->matShininess = matShininess;

    setupMesh();
  }
  // Отрисовка
  void Draw(Shader &shader) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;

    shader.setFloat("material.shininess", matShininess);

    // Текстурные карты
    for (unsigned int i = 0; i < textures.size(); i++) {
      std::string number;
      std::string name = textures[i].type;
      if (name == "texture_diffuse")
        number = std::to_string(++diffuseNr);
      else if (name == "texture_specular")
        number = std::to_string(++specularNr);
      else if (name == "texture_normal")
        number = std::to_string(++normalNr);
      else if (name == "texture_height")
        number = std::to_string(++heightNr);

      shader.setInt(("material." + name + number).c_str(), (int)i);
      glBindTextureUnit(i, textures[i].id);
    }

    // Привязка VAO к текущему контексту
    glBindVertexArray(VAO);
    // Отрисовка элементов
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()),
                   GL_UNSIGNED_INT, 0);
    // Отвязка VAO от текущего контекста
    glBindVertexArray(0);
  }

private:
  // Данные рендера
  unsigned int VBO, EBO;

  void setupMesh() {
    // Создание имен VAO, VBO и EBO
    // ----------------------------
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    // Заполнение буферов данными
    // --------------------------
    // VBO
    glNamedBufferStorage(VBO, vertices.size() * sizeof(Vertex), &vertices[0],
                         0);
    // EBO
    glNamedBufferStorage(EBO, indices.size() * sizeof(unsigned int),
                         &indices[0], 0);

    // Атрибуты вершин
    // ---------------
    /* Позиции */
    // Формат
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, Position));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 0, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 0);

    /* Нормали */
    // Формат
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, Normal));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 1, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 1);

    /* Текстурные координаты */
    // Формат
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, TexCoords));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 2, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 2);

    /* Tangent */
    // Формат
    glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, Tangent));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 3, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 3);

    /* Bitangent */
    // Формат
    glVertexArrayAttribFormat(VAO, 4, 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, Bitangent));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 4, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 4);

    /* ids */
    // Формат
    glVertexArrayAttribFormat(VAO, 5, 4, GL_INT, GL_FALSE,
                              offsetof(Vertex, m_BoneIDs));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 5, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 5);

    /* ids */
    // Формат
    glVertexArrayAttribFormat(VAO, 6, 4, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, m_Weights));
    // Прикрепление атрибута к VAO
    glVertexArrayAttribBinding(VAO, 6, 0);
    // Включение
    glEnableVertexArrayAttrib(VAO, 6);

    // Прикрепление буферов VAO
    // ------------------------
    // VBO
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
    // EBO
    glVertexArrayElementBuffer(VAO, EBO);
  }
};

#endif
