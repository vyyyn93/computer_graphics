#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "common/GLShader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };

struct Vertex {
    vec2 position;
    vec3 color;
    vec2 texCoord;
};

struct Application {
    GLShader g_BasicShader;
    GLuint VBO, IBO, VAO;
    GLuint texture1ID, texture2ID;

    void Initialize() {
        // Chargement et compilation des shaders
        g_BasicShader.LoadVertexShader("shader/basic.vs.glsl");
        g_BasicShader.LoadFragmentShader("shader/basic3.fs.glsl");
        g_BasicShader.Create();

        const std::vector<Vertex> vertices = {
            { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }, // sommet 0
            { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },  // sommet 1
            { { 0.0f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } },   // sommet 2
        };

        const std::vector<unsigned int> indices = { 0, 1, 2 }; // Définition du tableau d'indices

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
        int loc_texCoord = glGetAttribLocation(g_BasicShader.GetProgram(), "a_texCoord");

        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(loc_position);

        glVertexAttribPointer(loc_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(loc_texCoord);

        // Chargement des textures
        glGenTextures(1, &texture1ID);
        glBindTexture(GL_TEXTURE_2D, texture1ID);

        // Texture 1 : Configuration et chargement
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int w1, h1;
        uint8_t *data1 = stbi_load("map_01.png", &w1, &h1, nullptr, STBI_rgb_alpha);
        if (data1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w1, h1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
            stbi_image_free(data1);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1, &texture2ID);
        glBindTexture(GL_TEXTURE_2D, texture2ID);

        // Texture 2 : Configuration et chargement
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int w2, h2;
        uint8_t data[] = { 255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,255,255 };
        // Notez que le format interne GL_RGBA8 est typé, chaque composant est de 8 bits
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Désactivation du VAO
        glBindVertexArray(0);
    }

    void Terminate() {
        g_BasicShader.Destroy();
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &texture1ID);
        glDeleteTextures(1, &texture2ID);
    }

    void Render(int width, int height) {
        // Etape a. Récupérer/passer les variables width/height
        glViewport(0, 0, width, height);
        // Etape b. Notez que glClearColor est un état, donc persistant
        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Etape c. On spécifie le shader program à utiliser
        auto basicProgram = g_BasicShader.GetProgram();
        glUseProgram(basicProgram);

        // Lier les textures aux unités de texture 0 et 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1ID);
        glUniform1i(glGetUniformLocation(basicProgram, "u_texture1"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2ID);
        glUniform1i(glGetUniformLocation(basicProgram, "u_texture2"), 1);

        // Etape e. Utilisation du VAO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Désactivation du VAO après le rendu
        glBindVertexArray(0);
    }
};

int main(void) {
    int width = 640;
    int height = 480;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Exemple OpenGL 2.+", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ICI !
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
        return -1;
    }

    Application app;
    app.Initialize();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
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
