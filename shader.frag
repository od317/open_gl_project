#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;
in vec3 Normal;

struct Light {
    vec3 position;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 10
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;
uniform float shininess = 32.0;

vec3 calculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectColor) {
    // Ambient
    vec3 ambient = light.ambient * light.color * objectColor;
    
    // Diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * light.color * objectColor;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec * light.color;
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Start with a base ambient (dark scene)
    vec3 result = vec3(0.1, 0.1, 0.15) * Color;
    
    // Calculate all lights
    for(int i = 0; i < numLights; i++) {
        result += calculateLight(lights[i], norm, FragPos, viewDir, Color);
    }
    
    // Gamma correction
    result = pow(result, vec3(1.0/2.2));
    
    FragColor = vec4(result, 1.0);
}