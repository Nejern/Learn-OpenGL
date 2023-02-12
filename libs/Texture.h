#ifndef TEXTURE_H
#define TEXTURE_H

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

class Texture {
public:
  GLuint ID;
  int width, height, nrChannels;

  Texture(const char *path) {
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
      if (nrChannels == 3) {
        glTextureStorage2D(ID, 1, GL_RGB8, width, height);
        glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGB,
                            GL_UNSIGNED_BYTE, data);
      } else if (nrChannels == 4) {
        glTextureStorage2D(ID, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RGBA,
                            GL_UNSIGNED_BYTE, data);
      }
    } else {
      std::cout << "Failed to load texture" << std::endl;
    }
    glGenerateTextureMipmap(ID);
    stbi_image_free(data);
  }

  ~Texture() { glDeleteTextures(1, &ID); }
};

#endif
