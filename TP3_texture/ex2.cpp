#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "common/GLShader.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
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

static void get_file_data(void* ctx, const char* filename, const int is_mtl,
                          const char* obj_filename, char** data, size_t* len) {
    (void)ctx;
    (void)is_mtl;
    (void)obj_filename;

    if (!filename) {
        fprintf(stderr, "null filename\n");
        (*data) = NULL;
        (*len) = 0;
        return;
    }
    
    // Ouvrir le fichier en mode binaire
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("fopen r");
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    // Se déplacer à la fin du fichier pour obtenir sa taille
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(file);
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    long file_size = ftell(file);
    if (file_size == -1) {
        perror("ftell");
        fclose(file);
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    // Revenir au début du fichier
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
        fclose(file);
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    // Allouer de la mémoire pour le contenu du fichier
    *data = (char*)malloc(file_size);
    if (!*data) {
        fprintf(stderr, "Failed to allocate memory\n");
        fclose(file);
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    // Lire le contenu du fichier dans la mémoire allouée
    size_t read_size = fread(*data, 1, file_size, file);
    if (read_size != file_size) {
        fprintf(stderr, "Failed to read file\n");
        free(*data);
        fclose(file);
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    // Fermer le fichier
    fclose(file);

    // Retourner la taille du fichier
    *len = file_size;
}

void loadObj(const char* filename, Mesh* mesh) {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, filename, get_file_data, NULL, flags);
    
    if (ret != TINYOBJ_SUCCESS) {
        fprintf(stderr, "Failed to load/parse .obj file\n");
        return;
    }

    mesh->vertexCount = attrib.num_faces * 3;
    mesh->vertices = (Vertex*)malloc(mesh->vertexCount * sizeof(Vertex));
    if (!mesh->vertices) {
        fprintf(stderr, "Failed to allocate memory for vertices\n");
        return;
    }
    
    for (size_t i = 0; i < attrib.num_faces; i++) {
        for (size_t j = 0; j < 3; j++) {
            tinyobj_vertex_index_t idx = attrib.faces[3 * i + j ];
            int v_idx = idx.v_idx;
            int vn_idx = idx.vn_idx;
            int vt_idx = idx.vt_idx;

            Vertex vertex;
            if (v_idx >= 0 && v_idx < attrib.num_vertices) {
                vertex.position.x = attrib.vertices[3 * v_idx + 0];
                vertex.position.y = attrib.vertices[3 * v_idx + 1];
                vertex.position.z = attrib.vertices[3 * v_idx + 2];
            } else {
                vertex.position.x = 0.0f;
                vertex.position.y = 0.0f;
                vertex.position.z = 0.0f;
            }

            if (vn_idx >= 0 && vn_idx < attrib.num_normals) {
                vertex.normal.x = attrib.normals[3 * vn_idx + 0];
                vertex.normal.y = attrib.normals[3 * vn_idx + 1];
                vertex.normal.z = attrib.normals[3 * vn_idx + 2];
            } else {
                vertex.normal.x = 0.0f;
                vertex.normal.y = 0.0f;
                vertex.normal.z = 0.0f;
            }

            if (vt_idx >= 0 && vt_idx < attrib.num_texcoords) {
                vertex.texcoords.x = attrib.texcoords[2 * vt_idx + 0];
                vertex.texcoords.y = attrib.texcoords[2 * vt_idx + 1];
            } else {
                vertex.texcoords.x = 0.0f;
                vertex.texcoords.y = 0.0f;
            }

            mesh->vertices[3 * i + j] = vertex;
        }
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
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

void createViewMatrix(float* matrix, vec3 position, vec3 target, vec3 up) {
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

void createModelMatrix(float* matrix, vec3 translation, vec3 rotation, vec3 scale) {
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

struct Application {
    GLShader m_basicProgram;

    uint32_t m_VAO;
    uint32_t m_VBO;
    GLuint textureID;
    Mesh mesh;

    void Initialize() {
        m_basicProgram.LoadVertexShader("shader/suzanne.vs.glsl");
        m_basicProgram.LoadFragmentShader("shader/suzanne.fs.glsl");
        m_basicProgram.Create();

        loadObj("obj/cube2.obj", &mesh);

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
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

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

        glEnable(GL_CULL_FACE);

        glBindVertexArray(0);
    }

    void Terminate() {
        m_basicProgram.Destroy();
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteTextures(1, &textureID);
        free(mesh.vertices);
    }

    void Render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uint32_t program = m_basicProgram.GetProgram();
        if (program == 0) {
            std::cerr << "Program is not valid" << std::endl;
        }
        glUseProgram(program);

        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(program, "u_texture1"), 0);

        float projection[16];
        createPerspectiveMatrix(projection, 45.0f * (3.14159265358979323846 / 180.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        int projectionLoc = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);

        vec3 cameraPos = {0.0f, 0.0f, 3.0f};
        vec3 target = {0.0f, 0.0f, 0.0f};
        vec3 up = {0.0f, 1.0f, 0.0f};
        float view[16];
        createViewMatrix(view, cameraPos, target, up);
        int viewLoc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

        vec3 translation = {0.0f, 0.0f, -2.0f};
        vec3 rotation = {(float)glfwGetTime()*0.5f, (float)glfwGetTime()*0.5f, 0.0f};
        vec3 scale = {1.0f, 1.0f, 1.0f};
        float model[16];
        createModelMatrix(model, translation, rotation, scale);
        int modelLoc = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

        vec3 lightPos = {1.2f, 1.0f, 2.0f};
        vec3 viewPos = cameraPos;
        vec3 lightColor = {1.0f, 0.0f, 0.0f};
        vec3 objectColor = {1.0f, 1.0f, 1.0f};
        float shininess = 32.0f; 

        glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
        glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(program, "objectColor"), objectColor.x, objectColor.y, objectColor.z);
        glUniform1f(glGetUniformLocation(program, "shininess"), shininess);


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
