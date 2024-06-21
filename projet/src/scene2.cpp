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

//planete
double lastTime = glfwGetTime();;
double currentTime = 0.0;
double deltaTime = 0.0;

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

vec3 calculateNextCircularOrbitPosition(const vec3& currentPosition, float distance, float period, float deltaTime) {
    // Calculer l'angle actuel à partir de la position actuelle
    float currentAngle = calculateCurrentAngle(currentPosition);

    // Ajouter l'angle supplémentaire en fonction de la période et du deltaTime
    float angleIncrement = 2.0f * M_PI * (deltaTime / period);
    float newAngle = currentAngle + angleIncrement;

    // Calcul de la nouvelle position sur le cercle
    float x = distance * std::cos(newAngle);
    float z = distance * std::sin(newAngle);

    // Retourner la nouvelle position
    return vec3(x, currentPosition.y, z);
}

struct Application {
    const char* texture_soleil = "texture/soleil.jpg";
    My3DObject object1 = My3DObject(
            "obj/sphere.obj",
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {3.0f, 3.0f, 3.0f},
            {0.0f, 0.0f, 0.0f},
            "shader/soleil.vs.glsl",
            "shader/soleil.fs.glsl"
    );
    
    ///////////////////////////////
    My3DObject object2 = My3DObject(
            "obj/sphere.obj",
            {15.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            "shader/planet.vs.glsl",
            "shader/planet.fs.glsl"
    );
    float period2 = 20.0f;    
    vec3 init_pos2 = object2.getTranslation();
    float distance_object2 = calculateDistance(
        object1.getTranslation(),
        object2.getTranslation());
    
    //////////////////////////////////
    My3DObject object3 = My3DObject(
            "obj/sphere.obj",
            {-10.0f, 0.0f, 10.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            "shader/planet.vs.glsl",
            "shader/planet.fs.glsl"
    );
    float period3 = 20.0f;      
    vec3 init_pos3 = object3.getTranslation();
    float distance_object3 = calculateDistance(
        object1.getTranslation(),
        object3.getTranslation());
    
    /////////////////////////////////
    const char* texture_planete = "texture/planete.jpg";
    My3DObject object4 = My3DObject(
            "obj/sphere.obj",
            {8.0f, 0.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            "shader/planet.vs.glsl",
            "shader/planet.fs.glsl"
    );
    float period4 = 10.0f;      
    vec3 init_pos4 = object4.getTranslation();
    float distance_object4 = calculateDistance(
        object1.getTranslation(),
        object4.getTranslation());

    MyCamera camera = MyCamera();
    void Initialize() {
        //uint8_t skinSoleil[4] = {255, 255, 224, 255};
        object1.InitializeObject();
        object1.loadMipMapTexture(texture_soleil, 1.0f);
        
        uint8_t bleu[4] = {70, 130, 180, 255};
        object2.loadTexture(bleu);
        object2.InitializeObject();
        
        uint8_t planetTexture[] = {
            0, 0, 255, 255,    // Bleu pour les océans
            0, 255, 0, 255,    // Vert pour les forêts
            165, 42, 42, 255,  // Marron pour les montagnes
            255, 255, 255, 255 // Blanc pour les calottes glaciaires
        };
        object3.loadComplexeTexture(planetTexture);
        object3.InitializeObject();
        
        object4.InitializeObject();
        object4.loadMipMapTexture(texture_planete, 1.0f);

        glEnable(GL_CULL_FACE);
    }

    void Terminate() {
        object1.~My3DObject();
        object2.~My3DObject();
        object3.~My3DObject();
        object4.~My3DObject();
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

        vec3 lightPos = {0.0f, 0.0f, 0.0f};
        vec3 viewPos = camera.getCameraPosition();
        vec3 lightColor = {1.0f, 0.95f, 0.8f};
        float shininess = 50.0f; 

        glUniform3f(glGetUniformLocation(programID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(programID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(programID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(glGetUniformLocation(programID, "shininess"), shininess);

        vec3 ambient = {1.0f, 1.0f, 1.0f};
        vec3 skyDirection = {0.0f, 1.0f, 0.0f}; // Direction vers le ciel
        glUniform3fv(glGetUniformLocation(programID, "SkyColor"), 1, &ambient.x);
        glUniform3fv(glGetUniformLocation(programID, "GroundColor"), 1, &ambient.x);
        glUniform3fv(glGetUniformLocation(programID, "SkyDirection"), 1, &skyDirection.x);
        glUseProgram(0);
    }

    void Render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        object1.setRotation({0.0f, float(currentTime)/4, 0.0f});
        RenderObject(object1);

        vec3 newPosition = calculateNextCircularOrbitPosition(
            object2.getTranslation(), 
            distance_object2,
            period2, 
            deltaTime);
        object2.setTranslation(newPosition);
        RenderObject(object2);

        newPosition = calculateNextCircularOrbitPosition(
            object3.getTranslation(), 
            distance_object3,
            period3, 
            deltaTime);
        object3.setTranslation(newPosition);
        RenderObject(object3);
        
        newPosition = calculateNextCircularOrbitPosition(
            object4.getTranslation(), 
            distance_object4,
            period4, 
            deltaTime);
        object4.setTranslation(newPosition);
        object4.setRotation({0.0f, float(currentTime)/4, 0.0f});
        RenderObject(object4);
        
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
