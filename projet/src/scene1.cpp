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

//Balle
float ballPositionY = 5.0f;   // Position initiale de la balle en Y
float ballVelocityY = 0.0f;   // Vitesse initiale de la balle en Y
const float gravity = -9.8f;  // Accélération due à la gravité
const float restitution = 1.0f; // Coefficient de restitution (bounciness)
double lastTime = glfwGetTime();;
double currentTime = 0.0;
double deltaTime = 0.0;
const float minVelocity = 0.1f;

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

void updateBallPosition(float& ballPositionY, float& ballVelocityY, float deltaTime, float gravity, float restitution, float minVelocity) {
    ballVelocityY += gravity * deltaTime; 
    ballPositionY += ballVelocityY * deltaTime; 

    // Détecter un rebond
    if (ballPositionY <= 1.0f) {
        ballPositionY = 1.0f; 

        ballVelocityY = -ballVelocityY * restitution;

    }
}

struct Application {
    My3DObject object1 = My3DObject(
            "obj/human.obj",
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            "shader/human.vs.glsl",
            "shader/human.fs.glsl"
    );

    const char* texture_ballon = "texture/ballon.jpg";
    My3DObject object2 = My3DObject(
            "obj/sphere.obj",
            {0.0f, 1.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            "shader/ballon.vs.glsl",
            "shader/ballon.fs.glsl"
    );
    
    const char* texture_pelouse = "texture/grass2.jpg";
    My3DObject object3 = My3DObject(
            "obj/cube.obj",
            {0.0f, -1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {400.0f, 0.5f, 400.0f},
            {1.0f, 1.0f, 1.0f},
            "shader/ballon.vs.glsl",
            "shader/ballon.fs.glsl"
    );

    MyCamera camera = MyCamera();

    void Initialize() {
        uint8_t skinBeige[4] = { 245, 222, 179, 255 }; // R=245, G=222, B=179, A=255
        object1.loadTexture(skinBeige);
        object1.InitializeObject();

        object2.InitializeObject();
        object2.loadMipMapTexture(texture_ballon, 1.0f);
        
        object3.InitializeObject();
        object3.loadMipMapTexture(texture_pelouse, 50.f);

        glEnable(GL_CULL_FACE);
    }

    void Terminate() {
        object1.~My3DObject();
        object2.~My3DObject();
        object3.~My3DObject();
    }

    void RenderObject(const My3DObject& object) {
        uint32_t programID = object.getProgram().GetProgram();
        glUseProgram(programID);

        vec3 translation = object.getTranslation();
        vec3 rotation = object.getRotation();
        vec3 scale = object.getScale();
        float model[16];
        createWorldMatrix(model, translation, rotation, scale);
        int modelLoc = glGetUniformLocation(programID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        
        vec3 objectColor = object.getObjectColor();
        glUniform3f(glGetUniformLocation(programID, "objectColor"), objectColor.x, objectColor.y, objectColor.z);
        
        float texture_scale = object.getTextureScale();
        glUniform1f(glGetUniformLocation(programID, "texture_scale"), texture_scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, object.getTexture());
        glUniform1i(glGetUniformLocation(programID, "u_texture1"), 0);

        glBindVertexArray(object.getVAO());
        glBindBuffer(GL_ARRAY_BUFFER, object.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, object.getMesh().vertexCount);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        float projection[16];
        createPerspectiveMatrix(projection, 45.0f * (3.14159265358979323846 / 180.0f), 640.0f / 480.0f, 0.1f, 400.0f);
        int projectionLoc = glGetUniformLocation(programID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);

        camera.calculateCameraPosition(cameraDistance, cameraAzimuth, cameraElevation);
        float view[16];
        lookAt(view, camera.getCameraPosition(), camera.getCameraTarget(), camera.getCameraUp());
        int viewLoc = glGetUniformLocation(programID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        vec3 lightPos = {1000.0f, 500.0f, 1000.0f};
        vec3 viewPos = camera.getCameraPosition();
        vec3 lightColor = {1.0f, 0.95f, 0.8f};
        float shininess = 32.0f; 

        glUniform3f(glGetUniformLocation(programID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(programID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(programID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(glGetUniformLocation(programID, "shininess"), shininess);

        vec3 skyColor = {1.0f/4, 0.95f/4, 0.8f/4}; // Couleur du ciel
        vec3 groundColor = {0.05f/2, 0.35f/2, 0.05f/2}; // Couleur du sol
        vec3 skyDirection = {0.0f, 1.0f, 0.0f}; // Direction vers le ciel
        glUniform3fv(glGetUniformLocation(programID, "SkyColor"), 1, &skyColor.x);
        glUniform3fv(glGetUniformLocation(programID, "GroundColor"), 1, &groundColor.x);
        glUniform3fv(glGetUniformLocation(programID, "SkyDirection"), 1, &skyDirection.x);
        glUseProgram(0);
    }

    void Render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        RenderObject(object1);
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        updateBallPosition(ballPositionY, ballVelocityY, deltaTime, gravity, restitution, minVelocity);
        
        object2.setTranslation({0.0f, ballPositionY, 5.0f});
        RenderObject(object2);

        RenderObject(object3);
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
