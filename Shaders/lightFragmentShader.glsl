#version 460 core
out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {
  vec3 position;
  //vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

uniform mat4 view;

void main()
{
  float dist = length(light.position - FragPos);
  float attenuation = 1.f / (1.f + light.linear * dist + light.quadratic * pow(dist, 2));
  // ambient
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  ambient *= attenuation;

  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.f);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
  diffuse *= attenuation;

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
  specular *= attenuation;

  // result
  FragColor = vec4(ambient + diffuse + specular, 1.f);
}
