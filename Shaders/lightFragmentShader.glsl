#version 460 core
out vec4 FragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};
uniform Material material;

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform DirLight dirLight;

struct PointLight {
  vec3 position;

  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
#define MAX_POINT_LIGHTS 10
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform uint acutalPointLights;

struct SpotLight {
  vec3 position;
  vec3 direction;

  float cutOff;
  float outerCutOff;

  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform SpotLight spotLight;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

// Декларация функций
// ------------------
// Функция подсчета направленного света
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
// Функция подсчета направленного света
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragDir, vec3 viewDir);
// Функция подсчета направленного света
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  vec3 result = vec3(0.f);

  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  // Направленный свет
  if (dirLight.ambient != vec3(0.f) || dirLight.diffuse != vec3(0.f) || dirLight.specular != vec3(0.f)) {
    result = CalcDirLight(dirLight, norm, viewDir);
  }

  // Точечный свет
  for (int i = 0; i < acutalPointLights && i < MAX_POINT_LIGHTS; i++) {
    if (pointLights[i].ambient != vec3(0.f) || pointLights[i].diffuse != vec3(0.f) || pointLights[i].specular != vec3(0.f)) {
      result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
  }

  // "Прожекторный" свет
  if (spotLight.ambient != vec3(0.f) || spotLight.diffuse != vec3(0.f) || spotLight.specular != vec3(0.f)) {
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
  }

  FragColor = vec4(result, 1.f);
}

// Реализация функций
// ------------------
// Функция подсчета направленного света
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  // diffuse
  float diff = max(dot(normal, lightDir), 0.f);

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);

  // Результат
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

  return (ambient + diffuse + specular);
}

// Функция подсчета точечного света
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  // diffuse
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.f);

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);

  // attenuation
  float dist = length(light.position - fragPos);
  float attenuation = 1.f / (1.f + light.linear * dist + light.quadratic * pow(dist, 2));

  // Результат
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
  vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}

// Функция подсчета прожектерного света
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 result = vec3(0.f);
  vec3 lightDir = normalize(light.position - fragPos);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

  // attenuation
  float dist = length(light.position - fragPos);
  float attenuation = 1.f / (1.f + light.linear * dist + light.quadratic * pow(dist, 2));

  // diffuse
  result += light.diffuse * max(dot(normal, lightDir), 0.f) * texture(material.diffuse, TexCoords).rgb;

  // ambient
  result += light.ambient * texture(material.diffuse, TexCoords).rgb;

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  result += light.specular * pow(max(dot(viewDir, reflectDir), 0.f), material.shininess) * texture(material.specular, TexCoords).rgb;

  result *= attenuation * intensity;

  return result;
}
