#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;  // Ajoutez un uniforme pour la brillance

vec3 calculateAmbient(vec3 lightColor, float ambientStrength) {
    return ambientStrength * lightColor;
}

vec3 calculateDiffuse(vec3 normal, vec3 lightDir, vec3 lightColor) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor;
}

vec3 calculateSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 lightColor, float specularStrength, float shininess) {
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), shininess);
    return specularStrength * spec * lightColor;
}

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = calculateAmbient(lightColor, ambientStrength);
    
    // Normalized vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Diffuse 
    vec3 diffuse = calculateDiffuse(norm, lightDir, lightColor);
    
    // Specular
    float specularStrength = 0.5;
    vec3 specular = calculateSpecular(norm, lightDir, viewDir, lightColor, specularStrength, shininess);
    
    // Combine results
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}