#ifndef SHADOWRAY_H
#define SHADOWRAY_H

#include "structs.h"

/*
 * Handles the shadow effect in the final image
 * if any object is blocking the path to a light, that portion of the image is
 * darkened.
 *
 * Parameters:
 *  surface - the surface vector of the interesected object
 *  light - the light that may or may not be obscured
 *  is_sphere - tells if the intersected object is an sphere or face
 *  array_location - location in array of intersected object
 *  objects - struct containing all objects in enivronment
 * 
 * Returns:
 *   A float that represents how obscured the path from the intersection to the 
 *   light is.
 * 
 */
float shadowRay(vector_t surface, light_t light, int is_sphere, int array_location,
                objects_t objects);

#endif // SHADOWRAY_H