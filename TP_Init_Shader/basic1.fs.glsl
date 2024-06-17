#version 120

uniform float u_Time;

// un FS doit toujours ecrire dans 
// gl_FragColor qui est un vec4

varying vec3 v_Color;

void main()
{
    gl_FragColor = vec4(v_Color, 1.0);
}