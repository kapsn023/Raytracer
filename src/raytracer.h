#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "structs.h"
#include "intersections.h"

/*
 * Traces a ray and checks for intersections
 * intersection checks are handled completely by two helper functions
 * 
 * Parameters:
 *   ray - the ray that is to be traced
 *   depth - the current level of recursive depth
 *   objects - struct containing all objects in environment
 * 
 * Returns:
 *   A vector struct with the color information collected by the rays
 */


vector_t traceRay(ray_t ray, int depth, objects_t objects);

/*
 * Calculates the color information for each ray
 * Can spawn new rays if tranparency or reflection are involved
 * 
 * Parameters:
 *   - intersection - contains all of the information from the corresponding
 *   intersection
 *   - I - the direction of the incoming ray
 *   - is_sphere - informs function if intersected object is a sphere or a face
 *   - depth - a counter for the current level of recursive depth
 *   - objects - struct that contains all of the objects in the environment
 * 
 * Returns:
 *   A vector struct containing the color information for this intersection 
 *   and all of the recursive rays.
 */


vector_t shadeRay(intersection_t intersection, vector_t I, int is_sphere, int depth, objects_t objects);

#endif // RAYTRACER_H