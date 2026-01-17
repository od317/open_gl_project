#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

// Glass properties
uniform float transparency;
uniform float reflectivity;
uniform float refractionIndex;
uniform vec3 tintColor;

// Scene properties
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform samplerCube skybox; // For reflections (optional)
uniform sampler2D backBuffer; // For refraction (requires framebuffer)

// Fresnel reflectance function (Schlick's approximation)
float fresnelSchlick(float cosTheta, float n1, float n2) {
    float r0 = pow((n1 - n2) / (n1 + n2), 2.0);
    return r0 + (1.0 - r0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Normalize inputs
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate reflection
    vec3 reflectDir = reflect(-viewDir, norm);
    
    // Calculate refraction using Snell's law
    float ratio = 1.0 / refractionIndex;
    vec3 refractDir = refract(-viewDir, norm, ratio);
    
    // Sample reflection (from skybox or environment)
    vec3 reflection = texture(skybox, reflectDir).rgb;
    
    // Sample refraction (simplified - would need scene depth texture)
    vec3 refraction = vec3(0.7, 0.9, 1.0); // Default blue-ish tint
    
    // Calculate Fresnel effect (how much reflection vs refraction)
    float cosTheta = dot(-viewDir, norm);
    float fresnel = fresnelSchlick(cosTheta, 1.0, refractionIndex);
    
    // Mix reflection and refraction based on Fresnel
    vec3 surfaceColor = mix(refraction, reflection, fresnel * reflectivity);
    
    // Apply glass tint
    surfaceColor *= tintColor;
    
    // Add some specular highlights (glass shines)
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 256.0);
    vec3 specular = spec * lightColor * 0.5;
    
    // Add edge darkening (glass is darker at edges)
    float edgeFactor = 1.0 - pow(1.0 - dot(norm, viewDir), 2.0);
    surfaceColor *= (0.7 + 0.3 * edgeFactor);
    
    // Combine with transparency
    FragColor = vec4(surfaceColor + specular, transparency);
}