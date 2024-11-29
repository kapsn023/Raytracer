#include <math.h>
#include <float.h>


#include "structs.h"
#include "shadowRay.h"
#include "intersections.h"

// Handles the shadow effect in the final image
float shadowRay(vector_t surface, light_t light, int is_sphere, int array_location,
                objects_t objects){
    float output = 1.0;

    ray_t ray;
    ray.origin = surface;
    ray.direction = light.origin;
    if (light.w == 0){
        ray.direction = light.origin;
        vectorMult(&ray.direction, -1);
        vectorMult(&ray.direction, 1.0 / vectorLength(ray.direction));
    }
    else {
        ray.direction = surface;
        vectorMult(&ray.direction, -1);
        vectorAddVector(&ray.direction, light.origin);
        vectorMult(&ray.direction, 1.0 / vectorLength(ray.direction));
    }

    // Cycles through all spheres looking for intersections
    for (int i = 0; i < objects.sphereCounter; i++){
        if (!is_sphere || i != array_location){     // Ignores current sphere

            sphere_helper_t helper = sphereIntersectCheck(ray, objects.spheres[i]);
            
            // if there is an intersection, 
            // the impact receives less light
            if ((helper.min) >= 0){
                if (light.w == 0 && (helper.t1 > 0 || helper.t2 > 0)){ // Directional
                    output = output * (1.0 - objects.colors[objects.spheres[i].color].alpha);
                }
                else if (light.w == 1 && (helper.t1 > 0 || helper.t2 > 0) &&     // Point
                (helper.t1 < vectorLength(light.origin) && helper.t2 < vectorLength(light.origin))){ // checks for distance
                    output = output * (1.0 - objects.colors[objects.spheres[i].color].alpha);
                }
            }
        }
    }

    // Cylces through all faces looking for intersections
    for (int i = 0; i < objects.faceCounter; i++){
        if (is_sphere || i != array_location){ // ignores current face

            face_helper_t helper = faceIntersectCheck(ray, objects.faces[i]);

            // If there is an intersection, the final pixel color is dulled 
            // corresponding to how obscured the light is
            if (helper.t >= 0 && helper.alpha >= 0 && helper.alpha <= 1 && helper.beta >= 0 &&
                helper.beta <= 1 && helper.gamma >= 0 && helper.gamma <= 1){
                    
                output = output * (1.0 - objects.colors[objects.faces[i].color].alpha);
            }
        }
    }

    return output;
}