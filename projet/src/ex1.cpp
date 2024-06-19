#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "MyMatrixFunction.h"
#include "MyCamera.h"
#include "My3DStruct.h"
#include "common/GLShader.h"
#include "My3DObject.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tiny_obj_loader.h"

// Variables globales pour la caméra
float cameraDistance = 5.0f;
float cameraAzimuth = 0.0f;
float cameraElevation = 0.0f;
float lastX = 320, lastY = 240; 
bool firstMouse = true;


static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Notez que l'axe y est inversé
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.002f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraAzimuth += xoffset;
    cameraElevation += yoffset;

    // Limiter les angles d'élévation pour éviter des rotations incorrectes
    if (cameraElevation > 3.14f / 2.0f)
        cameraElevation = 3.14f / 2.0f;
    if (cameraElevation < -3.14f / 2.0f)
        cameraElevation = -3.14f / 2.0f;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance -= yoffset;
    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;
}

struct Application {
    GLShader m_basicProgram;
    GLuint texture1ID;
    uint32_t m_VAO1;
    uint32_t m_VBO1;
    My3DObject object1 = My3DObject(
            "obj/human.obj",
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            "texture/fond-briques.jpg"
    );

    
    GLuint texture2ID;
    uint32_t m_VAO2;
    uint32_t m_VBO2;
    My3DObject object2 = My3DObject(
            "obj/sphere.obj",
            {5.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            "texture/ballon.jpg"
            
    );

    MyCamera camera = MyCamera();

    void Initialize() {
        m_basicProgram.LoadVertexShader("shader/ex3.vs.glsl");
        m_basicProgram.LoadFragmentShader("shader/ex3.fs.glsl");
        m_basicProgram.Create();
        GLuint program = m_basicProgram.GetProgram();

        // Initialiser les objets
        InitializeObject(object1, m_VAO1, m_VBO1);
        InitializeObject(object2, m_VAO2, m_VBO2);

        // Charger les textures
        texture1ID = object1.getTexture();
        texture2ID = object2.getTexture();

        glEnable(GL_CULL_FACE);
    }

    void InitializeObject(My3DObject& object, uint32_t& VAO, uint32_t& VBO) {
        Mesh mesh = object.getMesh();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            mesh.vertexCount * sizeof(Vertex),
            mesh.vertices,
            GL_STATIC_DRAW
        );

        GLuint program = m_basicProgram.GetProgram();
        int positionLocation = glGetAttribLocation(program, "a_position");
        int normalLocation = glGetAttribLocation(program, "a_normal");
        int texcoordsLocation = glGetAttribLocation(program, "a_texcoords");

        if (positionLocation == -1 || normalLocation == -1 || texcoordsLocation == -1) {
            std::cerr << "Could not find attributes in the shader" << std::endl;
        }

        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(normalLocation);
        glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(texcoordsLocation);
        glVertexAttribPointer(texcoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

        glBindVertexArray(0);
    }

    void Terminate() {
        m_basicProgram.Destroy();
        glDeleteVertexArrays(1, &m_VAO1);
        glDeleteBuffers(1, &m_VBO1);
        glDeleteVertexArrays(1, &m_VAO2);
        glDeleteBuffers(1, &m_VBO2);
        glDeleteTextures(1, &texture1ID);
        glDeleteTextures(1, &texture2ID);
    }

    void RenderObject(const My3DObject& object, uint32_t VAO, GLuint textureID, GLuint program) {
        vec3 translation = object.getTranslation();
        vec3 rotation = object.getRotation();
        vec3 scale = object.getScale();
        float model[16];
        createWorldMatrix(model, translation, rotation, scale);
        int modelLoc = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

        vec3 objectColor = object.getObjectColor();
        glUniform3f(glGetUniformLocation(program, "objectColor"), objectColor.x, objectColor.y, objectColor.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(program, "u_texture1"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, object.getMesh().vertexCount);
        glBindVertexArray(0);
    }

    void Render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        uint32_t program = m_basicProgram.GetProgram();
        if (program == 0) {
            std::cerr << "Program is not valid" << std::endl;
        }
        glUseProgram(program);

        float projection[16];
        createPerspectiveMatrix(projection, 45.0f * (3.14159265358979323846 / 180.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        int projectionLoc = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);

        camera.calculateCameraPosition(cameraDistance, cameraAzimuth, cameraElevation);
        float view[16];
        lookAt(view, camera.getCameraPosition(), camera.getCameraTarget(), camera.getCameraUp());
        int viewLoc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        vec3 lightPos = {1.2f, 1.0f, 2.0f};
        vec3 viewPos = camera.getCameraPosition();
        vec3 lightColor = {1.0f, 0.95f, 0.8f};
        float shininess = 32.0f; 

        glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(glGetUniformLocation(program, "shininess"), shininess);

        vec3 skyColor = {1.0f/4, 0.95f/4, 0.8f/4}; // Couleur du ciel
        vec3 groundColor = {0.3f, 0.2f, 0.1f}; // Couleur du sol
        vec3 skyDirection = {0.0f, 1.0f, 0.0f}; // Direction vers le ciel
        glUniform3fv(glGetUniformLocation(program, "SkyColor"), 1, &skyColor.x);
        glUniform3fv(glGetUniformLocation(program, "GroundColor"), 1, &groundColor.x);
        glUniform3fv(glGetUniformLocation(program, "SkyDirection"), 1, &skyDirection.x);

        RenderObject(object1, m_VAO1, object1.getTexture(), program);
        RenderObject(object2, m_VAO2, object2.getTexture(), program);

        glUseProgram(0);
    }
};

int main(void) {
    GLFWwindow* window;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Exemple OpenGL 3.+", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(error) << std::endl;
        return -1;
    }

    Application app;
    app.Initialize();

    while (!glfwWindowShouldClose(window)) {
        app.Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
   
    app.Terminate();
    glfwTerminate();
    return 0;
}
