#ifndef My3DStruct_H
#define My3DStruct_H

#include <stdint.h>

struct vec2 { float x, y; };
struct vec3 {
    float x, y, z;

    // Constructeurs
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Opérateur d'addition
    vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    // Opérateur de soustraction
    vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    // Opérateur de multiplication par un scalaire
    vec3 operator*(float scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    // Opérateur de division par un scalaire
    vec3 operator/(float scalar) const {
        return vec3(x / scalar, y / scalar, z / scalar);
    }
};
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