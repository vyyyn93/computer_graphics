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
    vec3 color;
};

struct Application
{
    GLShader g_BasicShader;
    GLuint VBO, IBO, VAO;
    void Initialize()
    {
        // Chargement et compilation des shaders
        g_BasicShader.LoadVertexShader("basic.vs.glsl");
        g_BasicShader.LoadFragmentShader("basic.fs.glsl");
        g_BasicShader.Create();

        const std::vector<Vertex> vertices =
            {
                { { -0.5f, -0.5f } , { 1.0f, 0.0f, 0.0f } },    // sommet 0
                { { 0.5f, -0.5f } , { 0.0f, 1.0f, 0.0f } },  // sommet 1
                { { 0.0f,  0.5f } , { 0.0f, 0.0f, 1.0f } },   // sommet 2
            };

        const std::vector<unsigned int> indices = {0, 1, 2}; // Définition du tableau d'indices

        // Initialisation du VBO et transfert des données des vertices
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Initialisation de l'IBO et transfert des données des indices
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Initialisation du VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Configuration des attributs du VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

        int loc_position = glGetAttribLocation(g_BasicShader.GetProgram(), "a_position");
        int loc_color = glGetAttribLocation(g_BasicShader.GetProgram(), "a_color");

        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(loc_position);

        glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(loc_color);

        // Désactivation du VAO
        glBindVertexArray(0);
    }

    void Terminate() {
        g_BasicShader.Destroy();
        glDeleteBuffers(1, &VBO); 
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Render(int width, int height)
    {
        // Etape a. Récupérer/passer les variables width/height
        glViewport(0, 0, width, height);
        // Etape b. Notez que glClearColor est un état, donc persistant
        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Etape c. On spécifie le shader program à utiliser
        auto basicProgram = g_BasicShader.GetProgram();
        glUseProgram(basicProgram);

        // Etape d. Utilisation du VAO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Désactivation du VAO après le rendu
        glBindVertexArray(0);

        // Etape e. Non nécessaire ici
        // Etape f. Non nécessaire ici, glDrawArrays remplacé par glDrawElements
    }
};

int main(void)
{
    int width = 640;
    int height = 480;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Exemple OpenGL 2.+", NULL, NULL);
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
        app.Render(width, height);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    app.Terminate();

    glfwTerminate();
    
    return 0;
}
