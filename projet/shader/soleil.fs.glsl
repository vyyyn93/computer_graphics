#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D u_texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float shininess;

// Ajout des uniformes pour l'illumination hémisphérique
uniform vec3 SkyColor;
uniform vec3 GroundColor;
uniform vec3 SkyDirection;

vec3 calculateAmbient(vec3 norm) {
    float NdotSky = dot(norm, normalize(SkyDirection));
    float HemisphereFactor = NdotSky * 0.5 + 0.5;
    return mix(GroundColor, SkyColor, HemisphereFactor);
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
    // Normalisation de la normale
    vec3 norm = normalize(Normal);
    
    // Calcul de l'illumination ambiante hémisphérique
    vec3 ambient = calculateAmbient(norm);
    
    // Calcul de l'illumination diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = calculateDiffuse(lightColor, norm, lightDir);
    
    // Calcul de l'illumination spéculaire
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular = calculateSpecular(lightColor, norm, lightDir, viewDir, shininess);
    
    // Couleurs des textures
    vec3 color1 = texture(u_texture1, TexCoords).rgb;
    
    // Calcul de la couleur finale
    vec3 finalColor = (ambient + diffuse + specular) * color1;

    // Sortie de la couleur 
    color = vec4(finalColor, 1.0);
}
