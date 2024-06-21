#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float texture_scale;

void main() {
    FragPos = vec3(model * vec4(a_position, 1.0));
    Normal = mat3(transpose(inverse(model))) * a_normal;
    TexCoords = a_texcoords*texture_scale;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
