#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "common/GLShader.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct vec4 { float x, y, z, w; };

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoords;
};

struct Mesh {
    Vertex* vertices;
    uint32_t vertexCount;
};

void loadObj(const char* filename, Mesh* mesh) {
    std::string inputfile = filename;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret) {
        exit(1);
    }

    // Calculate the total number of vertices
    size_t totalVertices = 0;
    for (const auto& shape : shapes) {
        totalVertices += shape.mesh.indices.size();
    }

    mesh->vertexCount = static_cast<uint32_t>(totalVertices);
    mesh->vertices = (Vertex*)malloc(mesh->vertexCount * sizeof(Vertex));
    if (!mesh->vertices) {
        std::cerr << "Failed to allocate memory for vertices\n";
        return;
    }

    size_t vertexIndex = 0;
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shape.mesh.num_face_vertices[f]);
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Vertex& vertex = mesh->vertices[vertexIndex++];
                
                vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                if (idx.normal_index >= 0) {
                    vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texcoords.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vertex.texcoords.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                } 
            }
            index_offset += fv;
        }
    }
}

void createPerspectiveMatrix(float* matrix, float fov, float aspect, float near, float far) {
    float tanHalfFov = tan(fov / 2.0f);
    matrix[0] = 1.0f / (aspect * tanHalfFov);
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;

    matrix[4] = 0.0f;
    matrix[5] = 1.0f / tanHalfFov;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;

    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = -(2.0f * far * near) / (far - near);
    matrix[15] = 0.0f;
}

void lookAt(float* matrix, vec3 position, vec3 target, vec3 up) {
    vec3 f = {target.x - position.x, target.y - position.y, target.z - position.z};
    float f_length = sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
    f = {f.x / f_length, f.y / f_length, f.z / f_length};

    vec3 s = {f.y * up.z - f.z * up.y, f.z * up.x - f.x * up.z, f.x * up.y - f.y * up.x};
    float s_length = sqrt(s.x * s.x + s.y * s.y + s.z * s.z);
    s = {s.x / s_length, s.y / s_length, s.z / s_length};

    vec3 u = {s.y * f.z - s.z * f.y, s.z * f.x - s.x * f.z, s.x * f.y - s.y * f.x};

    matrix[0] = s.x;
    matrix[1] = u.x;
    matrix[2] = -f.x;
    matrix[3] = 0.0f;

    matrix[4] = s.y;
    matrix[5] = u.y;
    matrix[6] = -f.y;
    matrix[7] = 0.0f;

    matrix[8] = s.z;
    matrix[9] = u.z;
    matrix[10] = -f.z;
    matrix[11] = 0.0f;

    matrix[12] = -s.x * position.x - s.y * position.y - s.z * position.z;
    matrix[13] = -u.x * position.x - u.y * position.y - u.z * position.z;
    matrix[14] = f.x * position.x + f.y * position.y + f.z * position.z;
    matrix[15] = 1.0f;
}

void createWorldMatrix(float* matrix, vec3 translation, vec3 rotation, vec3 scale) {
    float cosX = cos(rotation.x);
    float sinX = sin(rotation.x);
    float cosY = cos(rotation.y);
    float sinY = sin(rotation.y);
    float cosZ = cos(rotation.z);
    float sinZ = sin(rotation.z);

    matrix[0] = scale.x * (cosY * cosZ);
    matrix[1] = scale.x * (cosY * sinZ);
    matrix[2] = scale.x * (-sinY);
    matrix[3] = 0.0f;

    matrix[4] = scale.y * (sinX * sinY * cosZ - cosX * sinZ);
    matrix[5] = scale.y * (sinX * sinY * sinZ + cosX * cosZ);
    matrix[6] = scale.y * (sinX * cosY);
    matrix[7] = 0.0f;

    matrix[8] = scale.z * (cosX * sinY * cosZ + sinX * sinZ);
    matrix[9] = scale.z * (cosX * sinY * sinZ - sinX * cosZ);
    matrix[10] = scale.z * (cosX * cosY);
    matrix[11] = 0.0f;

    matrix[12] = translation.x;
    matrix[13] = translation.y;
    matrix[14] = translation.z;
    matrix[15] = 1.0f;
}

float cameraDistance = 5.0f;
float cameraAzimuth = 0.0f;
float cameraElevation = 0.0f;
vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};

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

vec3 calculateCameraPosition(float distance, float azimuth, float elevation) {
    vec3 position;
    position.y = distance * sin(elevation);
    float radius = distance * cos(elevation);
    position.x = radius * cos(azimuth);
    position.z = radius * sin(azimuth);
    return position;
}

struct Application {
    GLShader m_basicProgram;

    uint32_t m_VAO;
    uint32_t m_VBO;
    GLuint texture1ID;
    Mesh mesh;

    void Initialize() {
        m_basicProgram.LoadVertexShader("shader/ex3.vs.glsl");
        m_basicProgram.LoadFragmentShader("shader/ex3.fs.glsl");
        m_basicProgram.Create();

        loadObj("obj/human.obj", &mesh);

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount * sizeof(Vertex), mesh.vertices, GL_STATIC_DRAW);

        int positionLocation = glGetAttribLocation(m_basicProgram.GetProgram(), "a_position");
        int normalLocation = glGetAttribLocation(m_basicProgram.GetProgram(), "a_normal");
        int texcoordsLocation = glGetAttribLocation(m_basicProgram.GetProgram(), "a_texcoords");

        if (positionLocation == -1 || normalLocation == -1 || texcoordsLocation == -1) {
            std::cerr << "Could not find attributes in the shader" << std::endl;
        }

        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(normalLocation);
        glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(texcoordsLocation);
        glVertexAttribPointer(texcoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

        // Chargement des textures
        glGenTextures(1, &texture1ID);
        glBindTexture(GL_TEXTURE_2D, texture1ID);

        // Texture 1 : Configuration et chargement
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int w1, h1;
        uint8_t *data1 = stbi_load("fond-briques.jpg", &w1, &h1, nullptr, STBI_rgb_alpha);
        if (data1) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w1, h1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
            stbi_image_free(data1);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        glEnable(GL_CULL_FACE);

        glBindVertexArray(0);
    }

    void Terminate() {
        m_basicProgram.Destroy();
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        free(mesh.vertices);
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

    // Lier les textures aux unités de texture 0 et 1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1ID);
    glUniform1i(glGetUniformLocation(program, "u_texture1"), 0);

    float projection[16];
    createPerspectiveMatrix(projection, 45.0f * (3.14159265358979323846 / 180.0f), 640.0f / 480.0f, 0.1f, 100.0f);
    int projectionLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);

    vec3 cameraPos = calculateCameraPosition(cameraDistance, cameraAzimuth, cameraElevation);
    float view[16];
    lookAt(view, cameraPos, cameraTarget, cameraUp);
    int viewLoc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    vec3 translation = {0.0f, 0.0f, 0.0f};
    //vec3 rotation = {(float)glfwGetTime(), (float)glfwGetTime(), 0.0f};
    vec3 rotation = {0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};
    float model[16];
    createWorldMatrix(model, translation, rotation, scale);
    int modelLoc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

    vec3 lightPos = {1.2f, 1.0f, 2.0f};
    vec3 viewPos = cameraPos;
    vec3 lightColor = {1.0f, 0.95f, 0.8f};
    vec3 objectColor = {1.0f, 1.0f, 1.0f};
    float shininess = 32.0f; 

    glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
    glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(program, "objectColor"), objectColor.x, objectColor.y, objectColor.z);
    glUniform1f(glGetUniformLocation(program, "shininess"), shininess);

    // Définir les nouvelles uniformes pour l'illumination hémisphérique
    vec3 skyColor = {1.0f/4, 0.95f/4, 0.8f/4}; // Couleur du ciel
    vec3 groundColor = {0.3f, 0.2f, 0.1f}; // Couleur du sol
    vec3 skyDirection = {0.0f, 1.0f, 0.0f}; // Direction vers le ciel
    glUniform3fv(glGetUniformLocation(program, "SkyColor"), 1, &skyColor.x);
    glUniform3fv(glGetUniformLocation(program, "GroundColor"), 1, &groundColor.x);
    glUniform3fv(glGetUniformLocation(program, "SkyDirection"), 1, &skyDirection.x);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
    glBindVertexArray(0);

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
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
