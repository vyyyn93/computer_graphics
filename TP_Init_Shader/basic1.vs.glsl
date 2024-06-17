#version 120

// un VS doit toujours ecrire dans
// gl_Position qui est un vec4

// attribute fait le lien avec
// glVertexAttrib ou glVertexAttribPointer
attribute vec2 a_Position; // canal ?

// valeur constante durant le rendu de la primitive
uniform float u_Time;

// varying fait le lien avec la variable
// du meme nom et type dans le fragment shader
varying vec3 v_Color;

void main()
{
	vec2 pos = a_Position;
    
	// on ajoute une valeur periodique afin d'animer notre triangle
	pos.x += sin(u_Time);
	
	// exemple de selection, on affecte pos.xy a v_Color.rb (eq. v_Color.xz)
	v_Color.rb = (pos + 1.0) / 2.0;

    gl_Position = vec4(pos, 0.0, 1.0);
}