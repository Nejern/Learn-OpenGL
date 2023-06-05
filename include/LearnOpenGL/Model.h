#ifndef MODEL_H
#define MODEL_H

// GLAD
#include "glad/gl.h"

// GLM
#include <glm/glm.hpp>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// Остальные заголовочные файлы
#include "Mesh.h"
#include "Shader.h"

// Объявление функции загрузки текстуры из файла
unsigned int TextureFromFile(const char *path, const std::string &directory,
                             bool gamma = false);

class Model {
public:
  // Данные модели
  // -------------
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;

  // Конструктор
  // -----------
  Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) {
    loadModel(path);
  }

  // Отрисовка
  // ---------
  void Draw(Shader &shader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
      meshes[i].Draw(shader);
  }

private:
  // Загрузка модели
  // ---------------
  void loadModel(std::string const &path) {
    // Чтение файла с помощью ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs |
                  aiProcess_CalcTangentSpace);

    // Проверка на ошибки
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
    }
    // Извлечение пути к каталогу
    directory = path.substr(0, path.find_last_of('/'));

    // Рекурсивная обработка корневого узла
    processNode(scene->mRootNode, scene);
  }

  // Рекурсивная обработка узла
  // --------------------------
  void processNode(aiNode *node, const aiScene *scene) {
    // Обработка всех узлов Mesh (если есть)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    // Обработка дочерних узлов
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
    }
  }

  Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
    // Данные для заполнения
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Обработка всех вершин
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 vector;
      /* Позиции */
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;
      /* Нормали */
      if (mesh->HasNormals()) {
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
      }
      /* Текстурные координаты */
      if (mesh->mTextureCoords[0]) {
        // Текстурные координаты
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
        // Tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        // Bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
      } else {
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      }
      vertices.push_back(vertex);
    }
    /* Индексы вершин */
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    /* Материал */
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // Diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // Specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // Normal maps
    std::vector<Texture> normalMaps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // Height maps
    std::vector<Texture> heightMaps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // Shininess
    float matShininess;
    if (AI_SUCCESS !=
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &matShininess)) {
      // if unsuccessful set a default
      matShininess = 16.f;
    }

    // Вывод
    return Mesh(vertices, indices, textures, matShininess);
  }

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str;
      mat->GetTexture(type, i, &str);
      bool skip = false;
      for (unsigned int j = 0; j < textures_loaded.size(); j++) {
        if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
          textures.push_back(textures_loaded[j]);
          skip = true;
          break;
        }
      }
      if (!skip) {
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), this->directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        textures_loaded.push_back(texture);
      }
    }
    return textures;
  }
};

unsigned int TextureFromFile(const char *path, const std::string &directory,
                             bool gamma) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

  stbi_set_flip_vertically_on_load(true);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum mainFormat;
    GLenum subFormat;
    if (nrComponents == 1) {
      mainFormat = GL_RED;
      subFormat = GL_RED;
    } else if (nrComponents == 3) {
      mainFormat = GL_RGB8;
      subFormat = GL_RGB;
    } else if (nrComponents == 4) {
      mainFormat = GL_RGBA8;
      subFormat = GL_RGBA;
    } else {
      std::cout << "Texture failed to load at path: " << path << std::endl;
      std::cout << "Because couldn't designate nrComponents" << std::endl;
    }

    glTextureStorage2D(textureID, 1, mainFormat, width, height);
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, subFormat,
                        GL_UNSIGNED_BYTE, data);
    glGenerateTextureMipmap(textureID);

    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
  stbi_image_free(data);

  return textureID;
}
#endif
