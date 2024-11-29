#ifndef STRUCTS_H
#define STRUCTS_H

// Vector Struct
typedef struct{
    float x, y, z;
} vector_t;

float vectorLength(vector_t u);
void crossProduct(vector_t* w, vector_t u, vector_t v);
void vectorMult(vector_t* w, float x);
void vectorAddVector(vector_t* w, vector_t u);
float dotProduct(vector_t v1, vector_t v2);

// Ray Struct
typedef struct{
    vector_t origin;
    vector_t direction;
} ray_t;

// Sphere Struct
typedef struct{
    vector_t origin;
    float rad;
    int color, texture;
    int count;
    int is_textured;
} sphere_t;

// Material Color Struct
typedef struct{
    vector_t diffuse;
    vector_t specular;
    float ka, kd, ks, n, alpha, eta;
} mtl_color_t;

// textures
typedef struct{
    vector_t* texture;
    int width, height;
} texture_t;

// lights
typedef struct{
    vector_t origin, color;
    float w;
} light_t;

// Texture Coordinates
typedef struct{
    float u, v;
} coord_t;

// Face struct
typedef struct{
    vector_t v0, v1, v2, vn0, vn1, vn2;
    coord_t vt0, vt1, vt2;
    int color, texture;
    int count;
    int is_smooth, is_textured;
} face_t;

// Environmental Objects 
typedef struct{
    sphere_t spheres[255];
    int sphereCounter;              // 0
    
    face_t faces[255]; 
    int faceCounter;                // 1

    light_t lights[255]; 
    int lightCounter;               // 0

    texture_t textures[255];
    int textureCounter;             // 0

    mtl_color_t colors[255]; 
    int mtlCounter;                 // -1

    float bkgr, bkgg, bkgb, eta;    // 1.0
} objects_t;

// Camera Info
typedef struct{
    // Image size
    int width, height;
    vector_t* image;

    // Eye coordinates
    vector_t eye;

    // Viewing direction coordinates
    vector_t vdir;

    // Up direction coordinates
    vector_t up;

    // Horizontal field of view
    float hfov;

    // View window
    float d;                       // 5.0
    vector_t ul, ur, ll, lr;

} camera_t;

#endif // STRUCTS_H