#version 330

// un VS doit toujours ecrire dans
// gl_Position qui est un vec4

// attribute fait le lien avec
// glVertexAttrib ou glVertexAttribPointer
in vec2 a_Position; 
in vec3 a_Color;

// valeur constante durant le rendu de la primitive
uniform float u_Time;

uniform mat4 u_RotationMatrix;
uniform mat4 u_TranslationMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

// varying/out fait le lien avec la variable
// du meme nom et type dans le fragment shader (ou l'etape suivante en OpenGL 3 avec out)
out vec3 v_Color;

void main()
{
	vec3 pos = vec3(a_Position, 0.0);

	v_Color = a_Color;

	mat4 RotationMatrix = mat4(
		cos(u_Time), sin(u_Time), 0.0, 0.0, // 1ere colonne
		-sin(u_Time), cos(u_Time), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);

	
	mat4 TranslationMatrix = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		sin(u_Time), 0.0 , 0.0, 1.0
	);

    gl_Position = TranslationMatrix * RotationMatrix * vec4(pos, 1.0);
}