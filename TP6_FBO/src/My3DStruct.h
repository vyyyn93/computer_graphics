#ifndef My3DStruct_H
#define My3DStruct_H

#include <stdint.h>

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

#endif