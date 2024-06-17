#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;

vec3 calculateAmbient(vec3 lightColor, float ambientStrength) {
    return ambientStrength * lightColor;
}

vec3 calculateDiffuse(vec3 lightColor, vec3 norm, vec3 lightDir) {
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * lightColor;
}

vec3 calculateSpecular(vec3 lightColor, vec3 norm, vec3 lightDir, vec3 viewDir, float shininess) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    return spec * lightColor * vec3(1.0); // Assuming Ks is 1.0 for simplicity
}

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = calculateAmbient(lightColor, ambientStrength);
    
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = calculateDiffuse(lightColor, norm, lightDir);
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular = calculateSpecular(lightColor, norm, lightDir, viewDir, 10);
    
    vec3 color1 = texture(u_texture1, TexCoords).rgb;
    vec3 color2 = texture(u_texture2, TexCoords).rgb;
    vec3 finalColor = (ambient + diffuse + specular) * color1;
    
    color = vec4(finalColor, 1.0);
}
