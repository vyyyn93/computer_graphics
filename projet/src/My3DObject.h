#ifndef My3DObject_H
#define My3DObject_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "My3DStruct.h"
#include <vector>
#include <cmath>
#include <iostream>
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

class My3DObject {
private:
    Mesh mesh;
    vec3 translation;
    vec3 rotation;
    vec3 scale;
    vec3 objectColor;
    GLuint textureID;
    GLShader program;

public:
    My3DObject(
        const char* filename, 
        vec3 translation, 
        vec3 rotation, 
        vec3 scale, 
        vec3 objectColor,
        const char* texturePath,
        const char* vertex_shader_path,
        const char* frag_shader_path
    ) : translation(translation), rotation(rotation), scale(scale), objectColor(objectColor) {
        loadObj(filename, &mesh);
        loadMipMapTexture(texturePath);
        loadProgram(vertex_shader_path, frag_shader_path );
    }

    ~My3DObject() {
        if (mesh.vertices) {
            free(mesh.vertices);
            mesh.vertices = nullptr;
        }
        glDeleteTextures(1, &textureID);
        program.Destroy();

    }

    void loadProgram(const char* vertex_shader_path, const char* frag_shader_path){
        program.LoadVertexShader(vertex_shader_path);
        program.LoadFragmentShader(frag_shader_path);
        program.Create();
    }

    void loadMipMapTexture(const char* texturePath) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int width, height, nrChannels;
        uint8_t *data = stbi_load(texturePath, &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
        }
    }

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

    vec3 getTranslation() const {
        return translation;
    }

    vec3 getRotation() const {
        return rotation;
    }

    vec3 getScale() const {
        return scale;
    }

    vec3 getObjectColor() const {
        return objectColor;
    }
    
    Mesh getMesh() const {
        return mesh;
    }

    GLuint getTexture() const {
        return textureID;
    }

    GLShader getProgram() const{
        return program;
    }
};

#endif
