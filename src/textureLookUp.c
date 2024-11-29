#include <math.h>
#include <float.h>

#include "structs.h"


// Handles the texture lookup for all textured spheres
vector_t faceTextureLookUp(face_t face, float alpha, float beta, float gamma, texture_t textures[]){
    float u = alpha * face.vt0.u + beta * face.vt1.u + gamma * face.vt2.u;
    float v = alpha * face.vt0.v + beta * face.vt1.v + gamma * face.vt2.v;

    int i = (int) (u * (textures[face.texture].width - 1));
    int j = (int) (v * (textures[face.texture].height - 1));

    return textures[face.texture].texture[(textures[face.texture].width * j) + i];
}

// Handles the texture lookup for all textured spheres
vector_t sphereTextureLookUp(sphere_t sphere, vector_t N, texture_t textures[]){
    vector_t output;
    
    float theta = atan2(N.y, N.x);
    float phi = acos(N.z);

    float u;
    if (theta > 0){
        u = theta / (2 * M_PI);
    }
    else {
        u = (theta + 2 * M_PI) / (2 * M_PI);
    }
    float v = phi / M_PI;

    int i = (int) (u * (textures[sphere.texture].width - 1));
    int j = (int) (v * (textures[sphere.texture].height - 1));

    output = textures[sphere.texture].texture[(textures[sphere.texture].width * j) + i];

    return output;
}