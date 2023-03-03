#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D texture_diffuse3;
  sampler2D texture_specular1;
  sampler2D texture_specular2;

  float shininess;
};
uniform Material material;

void main()
{    
    FragColor = texture(material.texture_specular1, TexCoords)
                + vec4(TexCoords, 0.f, 1.f);
}
