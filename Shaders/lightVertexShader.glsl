#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

uniform vec3 lightPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec2 TexCoords;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  FragPos = vec3(view * model * vec4(aPos, 1.0));
  Normal = normalMatrix * aNormal;
  LightPos = vec3(view * vec4(lightPos, 1.0));
  TexCoords = aTexCoords;
}
