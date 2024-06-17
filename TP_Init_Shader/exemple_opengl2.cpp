#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "common/GLShader.h"

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct vec4 { float x, y, z, w; };

struct Vertex
{
    vec2 position;
};

struct Application
{
    GLShader m_basicProgram;

    void Initialize()
    {
        m_basicProgram.LoadVertexShader("basic.vs.glsl");
        m_basicProgram.LoadFragmentShader("basic.fs.glsl");
        m_basicProgram.Create();
    }

    void Terminate()
    {
        m_basicProgram.Destroy();
    }

    void Render()
    { 
		//glClearColor(1.f, 1.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        uint32_t program = m_basicProgram.GetProgram();
        glUseProgram(program);

        double time = glfwGetTime();
        int timeLocation = glGetUniformLocation(program, "u_Time");
        glUniform1f(timeLocation, static_cast<float>(time));

		const std::vector<Vertex> triangles =
		{
			{ { 0.0f, 0.5f } },    // sommet 0
			{ { -0.5f, -0.5f } },  // sommet 1
			{ { 0.5f, -0.5f } },   // sommet 2
		};
		//GLuint == uint32_t
		//GLushort == uint16_t
		const uint16_t indices[] = { 0, 1, 2 };

		// {[x, y], [r, g, b]}
		const int POSITION = glGetAttribLocation(program, "a_Position");
		if (POSITION < 0)
			std::cout << "erreur de programme";
		glVertexAttribPointer(POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex)/*stride*/, triangles.data());
		glEnableVertexAttribArray(POSITION);

        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, indices);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Exemple OpenGL 2.+", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ICI !
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
    }

    Application app;
    app.Initialize();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        app.Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    app.Terminate();

    glfwTerminate();
    return 0;
}