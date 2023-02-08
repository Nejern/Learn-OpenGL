#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 colorFromVertex;
out vec2 texCoordFromVertex;

void main()
{
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
  colorFromVertex = color;
  texCoordFromVertex = texCoord;
}
