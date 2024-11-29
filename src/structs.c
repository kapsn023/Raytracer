#include <math.h>
#include <float.h>

#include "structs.h"

float vectorLength(vector_t u){
    return sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
}

void crossProduct(vector_t* w, vector_t u, vector_t v){
    w->x = (u.y * v.z - u.z * v.y);
    w->y = (u.z * v.x - u.x * v.z);
    w->z = (u.x * v.y - u.y * v.x);
}

void vectorMult(vector_t* w, float x){
    w->x *= x;
    w->y *= x;
    w->z *= x;
}
void vectorAddVector(vector_t* w, vector_t u){
    w->x += u.x;
    w->y += u.y;
    w->z += u.z;
}

float dotProduct(vector_t v1, vector_t v2){
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}