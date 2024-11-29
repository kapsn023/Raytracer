#ifndef INTERSECT_H
#define INTERSECT_H

#include "structs.h"

typedef struct{
    int exists;
    int index;
    float dist;
    vector_t point, N;
    mtl_color_t color;
} intersection_t;

typedef struct {
    float min, t1, t2;
} sphere_helper_t;

typedef struct{
    float alpha, beta, gamma, t;
    vector_t n, p;
} face_helper_t;

/*
 * Checks for intersections between a ray and a sphere
 *
 * Parameters: 
 *   ray - ray that is potentially intersecting with a sphere
 *   sphere - a single sphere that needs to be checked
 * 
 * Return:
 *   A intersection struct that contains all relevant information from the
 *   collison
 */

sphere_helper_t sphereIntersectCheck(ray_t ray, sphere_t sphere);

/*
 * Checks for intersections between a ray and a face
 *
 * Parameters: 
 *   ray - ray that is potentially intersecting with a face
 *   face - a single face that needs to be checked
 * 
 * Return:
 *   A intersection struct that contains all relevant information from the
 *   collison
 */

face_helper_t faceIntersectCheck(ray_t ray, face_t face);

/*
 * Checks for intersections between a ray and all spheres
 *
 * Parameters: 
 *   ray - ray that is potentially intersecting with spheres
 *   objects - struct containing all objects in environment
 * 
 * Return:
 *   A intersection struct that contains all relevant information from the
 *   collison
 */

intersection_t sphereIntersections(ray_t ray, objects_t objects);

/*
 * Checks for intersections between a ray and all faces
 *
 * Parameters: 
 *   ray - ray that is potentially intersecting with faces
 *   objects - struct containing all objects in environment
 * 
 * Return:
 *   A intersection struct that contains all relevant information from the
 *   collison
 */

intersection_t faceIntersections(ray_t ray, objects_t objects);

#endif // INTERSECT_H