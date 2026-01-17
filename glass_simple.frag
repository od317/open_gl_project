#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

// Glass properties
uniform float transparency;
uniform float reflectivity;
uniform vec3 tintColor;

// Scene lighting
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;

void main()
{
    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient lighting (prevents black areas)
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting (glass shininess)
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * reflectivity;
    
    // Edge effect (glass is more opaque at edges)
    float edgeFactor = 1.0 - pow(1.0 - dot(norm, viewDir), 2.0);
    float edgeOpacity = 0.4 * edgeFactor;
    
    // Base glass color with tint
    vec3 glassBase = vec3(0.9, 0.95, 1.0) * tintColor;
    
    // Combine lighting with glass color
    vec3 litGlass = (ambient + diffuse + specular) * glassBase;
    
    // Final alpha with edge effect
    float finalAlpha = transparency * (0.6 + 0.4 * edgeFactor);
    
    FragColor = vec4(litGlass, finalAlpha);
}