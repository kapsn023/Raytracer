#include <math.h>
#include <float.h>

#include "structs.h"
#include "raytracer.h"
#include "intersections.h"
#include "shadowRay.h"
#include "textureLookUp.h"

// Calculates the color information for each ray
// Can spawn new rays if tranparency or reflection are involved
vector_t shadeRay(intersection_t intersection, vector_t I, int is_sphere, int depth, objects_t objects){

    // Calculating ambient term
    vector_t output = intersection.color.diffuse;
    vectorMult(&output, intersection.color.ka);

    vector_t tempVector;

    for (int i = 0; i < objects.lightCounter; i++){
        vector_t light_output = objects.lights[i].color;
        vector_t L = objects.lights[i].origin;
        // Direction lights
        if (objects.lights[i].w == 0){
            vectorMult(&L, -1);
            vectorMult(&L, 1.0 / vectorLength(L));
        }
        // Point lights
        else {
            tempVector = intersection.point;
            vectorMult(&tempVector, -1);
            vectorAddVector(&L, tempVector);
            vectorMult(&L, 1.0 / vectorLength(L));
        }

        // Calculating H
        vector_t H = L;
        vectorAddVector(&H, I);
        vectorMult(&H, 1.0 / 2.0);
        vectorMult(&H, 1.0 / vectorLength(H));


        // Calculating diffuse term
        vector_t diffuse_output = intersection.color.diffuse;
        vectorMult(&diffuse_output, intersection.color.kd);

        float diffuse_angle = dotProduct(intersection.N, L);
        vectorMult(&diffuse_output, fmax(0.0, diffuse_angle));

        // Calculating specular term
        vector_t specular_output = intersection.color.specular;
        vectorMult(&specular_output, intersection.color.ks);

        float specular_angle = dotProduct(intersection.N, H);
        specular_angle = pow(specular_angle, intersection.color.n);
        vectorMult(&specular_output, fmax(0.0, specular_angle));
        
        // Combining terms
        vectorAddVector(&diffuse_output, specular_output);

        diffuse_output.x *= light_output.x;
        diffuse_output.y *= light_output.y;
        diffuse_output.z *= light_output.z;

        vectorMult(&diffuse_output, shadowRay(intersection.point, objects.lights[i], is_sphere, 
            intersection.index, objects));

        vectorAddVector(&output, diffuse_output);
    }

    // Reflection and Transperency
    if (depth < 10){
        float Fr, F0;
        F0 = pow((intersection.color.eta - 1.0)/(intersection.color.eta + 1.0), 2.0);
        Fr = F0 + (1 - F0) * pow((1 - fabs(dotProduct(intersection.N, I))), 5.0);
        //Reflection
        if (fabs(intersection.color.ks) > 0.001){
            vector_t R = intersection.N;
            tempVector = I;
            vectorMult(&R, 2 * dotProduct(intersection.N, I));
            vectorMult(&tempVector, -1.0);
            vectorAddVector(&R, tempVector);
            vectorMult(&R, 1.0 / vectorLength(R));

            ray_t tempRay;
            tempRay.direction = R;
            tempRay.origin = intersection.point;

            // Offset to avoid reflection acne
            tempVector = R;
            vectorMult(&tempVector, 0.1);
            vectorAddVector(&tempRay.origin, tempVector);

            tempVector = traceRay(tempRay, depth + 1, objects);

            vectorMult(&tempVector, Fr);

            vectorAddVector(&output, tempVector);
        }

        //Transperency
        vector_t T;
        vector_t N_neg = intersection.N;
        vectorMult(&N_neg, -1.0);
        // Outside an object
        if (dotProduct(intersection.N, I) >= 0 && (1 - pow((objects.eta/intersection.color.eta), 2.0) *
                (1 - pow(dotProduct(intersection.N, I), 2.0))) > 0){
            T = intersection.N;
            vectorMult(&T, -1.0);

            float x = sqrt(1 - pow((objects.eta/intersection.color.eta), 2.0) * 
                (1 - pow(dotProduct(intersection.N, I), 2.0)));
            vectorMult(&T, x);

            vector_t tempN, tempI;
            tempI = I;
            vectorMult(&tempI, -1.0);
            
            tempN = intersection.N;
            vectorMult(&tempN, dotProduct(intersection.N, I));

            vectorAddVector(&tempN, tempI);
            
            vectorMult(&tempN, objects.eta / intersection.color.eta);

            vectorAddVector(&T, tempN);
        }
        // Inside an object
        else if ((1 - pow((intersection.color.eta/objects.eta), 2.0) * 
            (1 - pow(dotProduct(N_neg, I), 2.0))) > 0) {

            T = N_neg;
            vectorMult(&T, -1.0);

            float x = sqrt(1 - pow((intersection.color.eta/objects.eta), 2.0) * 
                (1 - pow(dotProduct(N_neg, I), 2.0)));
            vectorMult(&T, x);

            vector_t tempN, tempI;
            tempI = I;
            vectorMult(&tempI, -1.0);
            
            tempN = N_neg;
            vectorMult(&tempN, dotProduct(N_neg, I));

            vectorAddVector(&tempN, tempI);
            
            vectorMult(&tempN, intersection.color.eta / objects.eta);

            vectorAddVector(&T, tempN);
        
        }

        ray_t tempRay;
        tempRay.direction = T;
        tempRay.origin = intersection.point;

        // Offset to avoid reflection acne
        tempVector = T;
        vectorMult(&tempVector, 0.1);
        vectorAddVector(&tempRay.origin, tempVector);

        tempVector = traceRay(tempRay, depth + 1, objects);

        vectorMult(&tempVector, ((1.0 - Fr) * (1.0 - intersection.color.alpha)));

        vectorAddVector(&output,tempVector);

    }

    return output;
}


// Traces a ray and checks for intersections
// intersection checks are handled completely by two helper functions
vector_t traceRay(ray_t ray, int depth, objects_t objects){
    vector_t outputColor;
    outputColor.x = objects.bkgr;
    outputColor.y = objects.bkgg;
    outputColor.z = objects.bkgb;
    vector_t I = ray.direction;
    vectorMult(&I, -1.0);
    float min = FLT_MAX;
    float t1;
    float t2;
    float b;
    float c;

    //Normalize the ray direction
    vectorMult(&ray.direction, 1.0/vectorLength(ray.direction));


    intersection_t sphere_intersection = sphereIntersections(ray, objects);

    intersection_t face_intersection = faceIntersections(ray, objects);

    if (sphere_intersection.exists && (!face_intersection.exists || 
        sphere_intersection.dist <= face_intersection.dist)){ 
    
        outputColor = shadeRay(sphere_intersection, I, 1, depth, objects);

    }
    else if (face_intersection.exists) {
        outputColor = shadeRay(face_intersection, I, 0, depth, objects);
    }

    return outputColor;
}