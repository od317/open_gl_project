#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform int shininess;

void main()
{
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Combine results
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}