#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};
uniform Material material;

struct Light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;

void main()
{
  vec3 lightIntens = vec3(5 / length(FragPos - LightPos));
  // ambient
  vec3 ambient = lightIntens * light.ambient * texture(material.diffuse, TexCoords).rgb;

  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(LightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = lightIntens * light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

  // specular
  vec3 viewDir = normalize(-FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = lightIntens * light.specular * spec * texture(material.specular, TexCoords).rgb;

  // result
  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
