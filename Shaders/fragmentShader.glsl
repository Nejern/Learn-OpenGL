#version 460 core

in vec3 colorFromVertex;
in vec2 texCoordFromVertex;

out vec4 color;

uniform sampler2D Texture_1;
uniform sampler2D Texture_2;

void main()
{
  color =
    mix(texture(Texture_1, texCoordFromVertex),
        texture(Texture_2, texCoordFromVertex),
        0.2);
}
