#include <math.h>
#include <float.h>
#include <stddef.h>


#include "structs.h"
#include "intersections.h"
#include "textureLookUp.h"

// Helper function that checks for intersections between a ray and a sphere
sphere_helper_t sphereIntersectCheck(ray_t ray, sphere_t sphere){
    sphere_helper_t output;

    output.min = FLT_MAX;

    // B = 2 * (xd *(x0–xc) + yd *(y0–yc) + zd *(z0–zc))
    // C = (x0–xc)^2 + (y0–yc)^2 + (z0–zc)^2 – r^2
    float b = 2.0 * (ray.direction.x * (ray.origin.x - sphere.origin.x) + ray.direction.y *
            (ray.origin.y - sphere.origin.y) + ray.direction.z * (ray.origin.z - sphere.origin.z));

    float c = (ray.origin.x - sphere.origin.x) * (ray.origin.x - sphere.origin.x)
        + (ray.origin.y - sphere.origin.y) * (ray.origin.y - sphere.origin.y) 
        + (ray.origin.z - sphere.origin.z) * (ray.origin.z - sphere.origin.z) 
        - sphere.rad * sphere.rad;

    // If ___ is greater zero, there are two valid solutions
    if ((b * b - 4 * c) > 0){
        output.t1 = (-b + sqrt(b * b - 4 * c)) / 2.0;
        output.t2 = (-b - sqrt(b * b - 4 * c)) / 2.0;

        if (output.t1 < output.t2 || output.t2 < 0){
            if (output.t1 < output.min && output.t1 > 0){
                    output.min = output.t1;
                }
            }
        else {
            if (output.t2 < output.min && output.t2 > 0){
                output.min = output.t2;
            }
        }
    }
    // if __ is zero, there is one solution
    else if ((b * b - 4 * c) == 0){
        float t1 = -b / 2.0;
        if (t1 < output.min && t1 > 0){
            output.min = t1;
        }
    }
    // else there are no valid solutions and an intersection has not occurred
    else {
        output.min = -1.0;
    }

    return output;
}

// Checks for intersections between a ray and all spheres
intersection_t sphereIntersections(ray_t ray, objects_t objects){
    intersection_t intersection;
    intersection.exists = 0;


    float min = FLT_MAX;
    float t1;
    float t2;

    for (int i = 0; i < objects.sphereCounter; i++){

        // checks if the current sphere being checked is closer than any other 
        // previous collisions
        float new_dist = sphereIntersectCheck(ray, objects.spheres[i]).min;

        if (new_dist  >= 0 && new_dist < min){
            min = new_dist;
            intersection.point = ray.direction;
            vectorMult(&intersection.point, min);
            vectorAddVector(&intersection.point, ray.origin);

            intersection.exists = 1;
            intersection.index = i;
        }
    }

    // Only does normal calculation if an intersection has occurred
    if (intersection.exists){

        intersection.N = intersection.point;
        vector_t tempVector = objects.spheres[intersection.index].origin;
        vectorMult(&tempVector, -1);
        vectorAddVector(&intersection.N, tempVector);
        vectorMult(&intersection.N, 1.0 / objects.spheres[intersection.index].rad);
        vectorMult(&intersection.N, 1.0 / vectorLength(intersection.N));

        intersection.color = objects.colors[objects.spheres[intersection.index].color];

        if (objects.spheres[intersection.index].is_textured){
            intersection.color.diffuse = sphereTextureLookUp(objects.spheres[intersection.index],
            intersection.N, objects.textures);
        }
    }
    intersection.dist = min;

    return intersection;   
}


// Helper function that checks for intersections between a ray and a face
face_helper_t faceIntersectCheck(ray_t ray, face_t face){
    face_helper_t output;

    vector_t e1, e2, ep;
    output.alpha = -1.0;
    output.beta = -1.0;
    output.gamma = -1.0;
    float D;

    e1 = face.v0;
    vectorMult(&e1, -1);
    vectorAddVector(&e1, face.v1);

    e2 = face.v0;
    vectorMult(&e2, -1);
    vectorAddVector(&e2, face.v2);

    crossProduct(&output.n, e1, e2);

    
    float denominator = output.n.x * ray.direction.x + output.n.y * ray.direction.y + 
        output.n.z * ray.direction.z;
    if (denominator != 0){
        D = - (output.n.x * face.v1.x + output.n.y * face.v1.y + output.n.z * face.v1.z);

        output.t = -(output.n.x * ray.origin.x + output.n.y * ray.origin.y + 
            output.n.z * ray.origin.z + D) / denominator;

        output.p = ray.direction;
        vectorMult(&output.p, output.t);
        vectorAddVector(&output.p, ray.origin);

        ep = face.v0;
        vectorMult(&ep, -1);
        vectorAddVector(&ep, output.p);

        // If det is not zero, that means that the ray intersected with the
        // same plane that the face lies upon, i.e. the ray is not parallel with
        // this plane
        float det = dotProduct(e1, e1) * dotProduct(e2, e2) - dotProduct(e1, e2) * dotProduct(e1, e2);
        if (det != 0){
            output.beta = (dotProduct(e2, e2) * dotProduct(e1, ep) - dotProduct(e1, e2) *
                dotProduct(e2, ep)) / det;
            output.gamma = (dotProduct(e1, e1) * dotProduct(e2, ep) - dotProduct(e1, e2) *
                dotProduct(e1, ep)) / det;
            output.alpha = 1 - (output.beta + output.gamma);
        }
    }

    return output;
}

// Checks for intersections between a ray and all faces
intersection_t faceIntersections(ray_t ray, objects_t objects){
    intersection_t intersection;
    intersection.exists = 0;

    float min = FLT_MAX;

    for (int i = 1; i < objects.faceCounter; i++){
        face_helper_t helper = faceIntersectCheck(ray, objects.faces[i]);

        // the helper function only returns intersections that occur with
        // the face's corresponding plane. This if statement checks if the 
        // intersections falls within the face's portion of the plane
        if (helper.alpha >= 0 && helper.alpha <= 1 && helper.beta >= 0 && helper.beta <= 1 &&
                helper.gamma >= 0 && helper.gamma <= 1 && helper.t < min && helper.t > 0){
            min = helper.t;
            
            if (objects.faces[i].is_smooth == 1){
                vector_t temp_vn1 = objects.faces[i].vn1;
                vector_t temp_vn2 = objects.faces[i].vn2;
                helper.n = objects.faces[i].vn0;
                vectorMult(&helper.n, helper.alpha);

                vectorMult(&temp_vn1, helper.beta);
                vectorMult(&temp_vn2, helper.gamma);

                vectorAddVector(&helper.n, temp_vn1);
                vectorAddVector(&helper.n, temp_vn2);
            }

            vectorMult(&helper.n, 1.0 / vectorLength(helper.n));

            intersection.color = objects.colors[objects.faces[i].color];

            if (objects.faces[i].is_textured){
                intersection.color.diffuse = faceTextureLookUp(objects.faces[i], helper.alpha, 
                    helper.beta, helper.gamma, objects.textures);
            }

            intersection.exists = 1;
            intersection.index = i;
            intersection.dist = min;
            intersection.point = helper.p;
            intersection.N = helper.n;
            
    
        }
        
    }

    return intersection;
}