#version 330 core
in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main() {
    vec4 color1 = texture(u_texture1, v_texCoord);
    vec4 color2 = texture(u_texture2, v_texCoord);
    FragColor = color2; // Mélange des deux textures
}
