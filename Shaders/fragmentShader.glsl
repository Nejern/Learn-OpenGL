#version 460 core

in vec3 colorFromVertex;
in vec2 texCoordFromVertex;

out vec4 color;

uniform sampler2D Texture;

void main()
{
  color = texture(Texture, texCoordFromVertex) * vec4(colorFromVertex, 1.0f);
}
