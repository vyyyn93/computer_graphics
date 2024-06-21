#ifndef MyMatrixFunction_H
#define MyMatrixFunction_H

#include "My3DStruct.h"
#include <cmath>

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

float calculateDistance(const vec3& a, const vec3& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + 
                     (a.y - b.y) * (a.y - b.y) + 
                     (a.z - b.z) * (a.z - b.z));
}

float calculateCurrentAngle(const vec3& position) {
    // Utiliser atan2 pour obtenir l'angle en radians
    return std::atan2(position.z, position.x);
}

#endif