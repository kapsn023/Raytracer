#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "structs.h"
#include "shadowRay.h"
#include "textureLookUp.h"
#include "raytracer.h"

objects_t objects;
camera_t camera;

/*
 * Handles that gathering of pixel information and writing this information to the file
 *
 * Parameters:
 *   output - the output file that will contain the image information
 * 
 * Returns:
 *   an integer that informs main() of any potential errors.
 */
int imageWriter(FILE *output){
    // Defining window offsets for pixels
    vector_t temp_vector = camera.ul;
    vector_t delta_h = camera.ur;
    vectorMult(&temp_vector, -1);
    vectorAddVector(&delta_h, temp_vector);
    vectorMult(&delta_h, 1.0 / (camera.width - 1));

    vector_t delta_v = camera.ll;
    vectorAddVector(&delta_v, temp_vector);
    vectorMult(&delta_v, 1.0 / (camera.height - 1));

    // Writing the header
    fprintf(output,"P3 %d %d 255\n", camera.width, camera.height);

    // Writing rgb information
    
    for (int j = 0; j < camera.height; j++){
        for (int i = 0; i < camera.width; i++){
            //fprintf(output, "127 0 255\n");
            ray_t tempRay;
            tempRay.origin = camera.eye;

            tempRay.direction = camera.ul;
            temp_vector = delta_h;
            vectorMult(&temp_vector, i);
            vectorAddVector(&tempRay.direction, temp_vector);

            temp_vector = delta_v;
            vectorMult(&temp_vector, j);
            vectorAddVector(&tempRay.direction, temp_vector);

            temp_vector = camera.eye;
            vectorMult(&temp_vector, -1.0);
            vectorAddVector(&tempRay.direction, temp_vector);
            
            vectorMult(&tempRay.direction, 1.0 / vectorLength(tempRay.direction));

            camera.image[(j * camera.width) + i] = traceRay(tempRay, 0, objects);
        }

    }

    for (int i = 0; i < (camera.width * camera.height); i++){
        fprintf(output, "%d %d %d\n", (int)(255 *fmin(1.0, camera.image[i].x)),
         (int)(255 * fmin(1.0, camera.image[i].y)), (int)(255 * fmin(1.0, camera.image[i].z)));
    }

    return 0;
}

/*
 * Reads the input file at updates values based on what is read
 * 
 * Parameter:
 *   input - the input file that is to be read
 * 
 * Return:
 *   an integer that informs main() of any potential errors.
 */

int fileReader(FILE *input){
    char str[32];
    memset(str, 0, 32);
    int scanOutput;

    // Vectors
    vector_t vectors[255];
    int vectorCounter = 1;

    // Normals
    vector_t normals[255];
    int normalCounter = 1;

    coord_t coords[255];
    int coordCounter = 1;

    int noImsize = 1;
    int noEye = 1;
    int noViewDir = 1;
    int noUpDir = 1;
    int noHFOV = 1;
    int noBKGColor = 1;

    int texture = 0;

    while (fscanf(input, " %s", str) != EOF){
        if (!strcmp(str, "imsize")){
            scanOutput = fscanf(input, " %d %d", &camera.width, &camera.height);
            if (scanOutput != 2 || camera.width <= 0 || camera.height <= 0){
                printf("Improper image format or size\n");
                return 1;
            }
            noImsize = 0;

        }
        else if (!strcmp(str, "eye")){
            scanOutput = fscanf(input, " %f %f %f", &camera.eye.x, &camera.eye.y, &camera.eye.z);
            if (scanOutput != 3){
                printf("Incorrect eye position format\n");
                return 1;
            }
            noEye = 0;

        }
        else if (!strcmp(str, "viewdir")){
            scanOutput = fscanf(input, " %f %f %f", &camera.vdir.x, &camera.vdir.y, &camera.vdir.z);
            if (scanOutput != 3){
                printf("Incorrect viewing direction format\n");
                return 1;
            }
            noViewDir = 0;

        }
        else if (!strcmp(str, "updir")){
            scanOutput = fscanf(input, " %f %f %f", &camera.up.x, &camera.up.y, &camera.up.z);
            if (scanOutput != 3){
                printf("Incorrect up direction format\n");
                return 1;
            }
            noUpDir = 0;

        }
        else if (!strcmp(str, "hfov")){
            scanOutput = fscanf(input, " %f", &camera.hfov);
            if (scanOutput != 1){
                printf("Incorrect horizontal field of view format\n");
                return 1;
            }
            noHFOV = 0;

        }
        else if (!strcmp(str, "bkgcolor")){
            scanOutput = fscanf(input, " %f %f %f %f ", &objects.bkgr, &objects.bkgg, 
                &objects.bkgb, &objects.eta);
            if (scanOutput != 4 || objects.bkgr < 0.0 || objects.bkgr > 1.0 || objects.bkgg < 0.0 ||
                objects.bkgg > 1.0 || objects.bkgb < 0.0 || objects.bkgb > 1.0){
                printf("Incorrect background color format\n");
                return 1;
            }
            noBKGColor = 0;
        }
        else if (!strcmp(str, "mtlcolor")){
            objects.mtlCounter++;
            scanOutput = fscanf(input, " %f %f %f %f %f %f %f %f %f %f %f %f ", 
             &objects.colors[objects.mtlCounter].diffuse.x, &objects.colors[objects.mtlCounter].diffuse.y,
             &objects.colors[objects.mtlCounter].diffuse.z, &objects.colors[objects.mtlCounter].specular.x,
             &objects.colors[objects.mtlCounter].specular.y, &objects.colors[objects.mtlCounter].specular.z,
             &objects.colors[objects.mtlCounter].ka, &objects.colors[objects.mtlCounter].kd, 
             &objects.colors[objects.mtlCounter].ks, &objects.colors[objects.mtlCounter].n, 
             &objects.colors[objects.mtlCounter].alpha, &objects.colors[objects.mtlCounter].eta);
            if (scanOutput != 12){
                printf("Incorrect material color format\n");
                return 1;
            }

        }
        else if (!strcmp(str, "sphere")){
            scanOutput = fscanf(input, " %f %f %f %f", &objects.spheres[objects.sphereCounter].origin.x ,
            &objects.spheres[objects.sphereCounter].origin.y, &objects.spheres[objects.sphereCounter].origin.z,
            &objects.spheres[objects.sphereCounter].rad);
            if (scanOutput != 4){
                printf("Incorrect sphere format\n");
                return 1;
            }
            objects.spheres[objects.sphereCounter].color = objects.mtlCounter;
            objects.spheres[objects.sphereCounter].count = objects.sphereCounter;
            if (texture){
                objects.spheres[objects.sphereCounter].is_textured = 1;
                objects.spheres[objects.sphereCounter].texture = objects.textureCounter - 1;
            }
            objects.sphereCounter++;

        }
        else if (!strcmp(str, "light")){
            scanOutput = fscanf(input, " %f %f %f %f %f %f %f", &objects.lights[objects.lightCounter].origin.x,
            &objects.lights[objects.lightCounter].origin.y, &objects.lights[objects.lightCounter].origin.z,
            &objects.lights[objects.lightCounter].w, &objects.lights[objects.lightCounter].color.x, 
            &objects.lights[objects.lightCounter].color.y, &objects.lights[objects.lightCounter].color.z);
            if (scanOutput != 7){
                printf("Incorrect light format\n");
                return 1;
            }
            objects.lightCounter++;

        }
        else if (!strcmp(str, "v")){
            scanOutput = fscanf(input, " %f %f %f ", &vectors[vectorCounter].x ,
            &vectors[vectorCounter].y, &vectors[vectorCounter].z);
            if (scanOutput != 3){
                printf("Incorrect vector format\n");
                return 1;
            }
            vectorCounter++;
        }
        else if (!strcmp(str, "vn")){
            scanOutput = fscanf(input, " %f %f %f ", &normals[normalCounter].x ,
            &normals[normalCounter].y, &normals[normalCounter].z);
            if (scanOutput != 3){
                printf("Incorrect normal format\n");
                return 1;
            }
            normalCounter++;
        }
        else if (!strcmp(str, "vt")){
            scanOutput = fscanf(input, " %f %f ", &coords[coordCounter].u ,
            &coords[coordCounter].v);
            if (scanOutput != 2){
                printf("Incorrect texture coordinate format\n");
                return 1;
            }
            coordCounter++;
        }
        else if (!strcmp(str, "texture")){
            char texture_name[255];
            int image_width, image_height, max_color;

            scanOutput = fscanf(input, " %s ", texture_name);
            FILE* texture_file = fopen(texture_name, "r");
            if (scanOutput != 1 || texture_file == NULL){
                printf("Texture file could not be opened\n");
                return 1;
            }
            texture = 1;

            if (fscanf(texture_file, "P3 %d %d %d ", &image_width, &image_height, &max_color) != 3){
                printf("Failed to read from texture file\n");
                fclose(texture_file);
                return 1;
            }
            objects.textures[objects.textureCounter].width = image_width;
            objects.textures[objects.textureCounter].height = image_height;
            objects.textures[objects.textureCounter].texture = 
                malloc(sizeof(vector_t) * image_width * image_height);

            int temp_r, temp_g, temp_b;
            for (int j = 0; j < image_height; j++){
                for (int i = 0; i < image_width; i++){
                    if (fscanf(texture_file, " %d %d %d ", &temp_r, &temp_g, &temp_b) != 3){
                        printf("Failed to read from texture file\n");
                        fclose(texture_file);
                        return 1;
                    }

                    objects.textures[objects.textureCounter].texture[j * image_width + i].x = 
                    (float)temp_r / (float)(max_color);
                    objects.textures[objects.textureCounter].texture[j * image_width + i].y = 
                    (float)temp_g / (float)(max_color);
                    objects.textures[objects.textureCounter].texture[j * image_width + i].z = 
                    (float)temp_b / (float)(max_color);
                }
            }
            fclose(texture_file);
            objects.textureCounter++;
        }
        else if (!strcmp(str, "f")) {
            char string[255];
            int x, y, z, vtx, vty, vtz, vnx, vny, vnz;
            fgets(string, 255, input);
            if (sscanf(string, "%d/%d/%d %d/%d/%d %d/%d/%d", 
                &x, &vtx, &vnx, &y, &vty, &vny, &z, &vtz, &vnz) == 9) {
            //success reading a face in v/t/n format; proceed accordingly
                objects.faces[objects.faceCounter].is_smooth = 1;
                objects.faces[objects.faceCounter].is_textured = 1;
                objects.faces[objects.faceCounter].texture = objects.textureCounter - 1;

                objects.faces[objects.faceCounter].vt0 = coords[vtx];
                objects.faces[objects.faceCounter].vt1 = coords[vty];
                objects.faces[objects.faceCounter].vt2 = coords[vtz];
            } 
            else if (sscanf(string, "%d//%d %d//%d %d//%d", &x, &vnx, &y, &vny, &z, &vnz) == 6) {
            //success reading a face in v//n format; proceed accordingly
                objects.faces[objects.faceCounter].is_smooth = 1;
                objects.faces[objects.faceCounter].is_textured = 0;

                objects.faces[objects.faceCounter].vn0 = normals[vnx];
                objects.faces[objects.faceCounter].vn1 = normals[vny];
                objects.faces[objects.faceCounter].vn2 = normals[vnz];
            } 
            else if (sscanf(string, "%d/%d %d/%d %d/%d", &x, &vtx, &y, &vty, &z, &vtz) == 6) {
            //success reading a face in v/t format; proceed accordingly
                objects.faces[objects.faceCounter].is_smooth = 0;
                objects.faces[objects.faceCounter].is_textured = 1;
                objects.faces[objects.faceCounter].texture = objects.textureCounter - 1;

                objects.faces[objects.faceCounter].vt0 = coords[vtx];
                objects.faces[objects.faceCounter].vt1 = coords[vty];
                objects.faces[objects.faceCounter].vt2 = coords[vtz];
            } 
            else if (sscanf(string, "%d %d %d", &x, &y, &z) == 3) {
            //success reading a face in v format; proceed accordingly
                objects.faces[objects.faceCounter].is_smooth = 0;
                objects.faces[objects.faceCounter].is_textured = 0;
            } else {
                //error reading face data
                printf("Error reading face data\n");
                return 1;
            }
            if (x > vectorCounter - 1 || y > vectorCounter -1 || z > vectorCounter - 1){
                printf("Vector does not exist yet.\n");
                return 1;
            }
            objects.faces[objects.faceCounter].v0 = vectors[x];
            objects.faces[objects.faceCounter].v1 = vectors[y];
            objects.faces[objects.faceCounter].v2 = vectors[z];

            objects.faces[objects.faceCounter].color = objects.mtlCounter;
            objects.faces[objects.faceCounter].count = objects.sphereCounter;
            objects.faceCounter++;
        }
        else {
            printf("Unrecognized keyword\n");
            return 1;
        }
    }

    // Checks to verify all needed inputs were read at least once
    if (noImsize || noEye || noViewDir || noUpDir || noHFOV || noBKGColor){
        printf("Lacking required information\n");
        return 1;
    }

    return 0;
}

void objects_init(){
    objects.sphereCounter = 0;
    objects.faceCounter = 1;
    objects.lightCounter = 0;
    objects.textureCounter = 0;
    objects.mtlCounter = -1;
    objects.bkgr, objects.bkgg, objects.bkgb, objects.eta = 1.0;
}

int main(int argc, char* argv[]){
    objects_init();

    FILE *inputFilePointer;
    FILE *outputFilePointer;

    int readerOutput;
    int writerOutput;

    char outputFileName[255] = "";
    char *token1;
    const char *delim = ".";

    camera.d = 5.0;

    // Viewing Coordinate System
    vector_t u, v, w;


    // Looks for input file and hands it off to fileReader
    // Checks for bad inputs
    if (argc == 2){
        inputFilePointer = fopen(argv[1], "r");
        if (inputFilePointer == NULL){
            printf("File not found\n");
            return 1;
        }
        else {
            readerOutput = fileReader(inputFilePointer);
            fclose(inputFilePointer);
            if (readerOutput){
                return 1;
            }
            
        }
    }
    else if (argc > 2){
        printf("Too many arguments.\n");
        return 1;
    }
    else {
        printf("Input file name required.\n");
        return 1;
    }

    camera.image = malloc(sizeof(vector_t) * (camera.width * camera.height));
    // u' = view_dir * up_dir, u = u' / ||u'||
    // w = u * v = (uy*vz-uz*vy, uz*vx-ux*vz, ux*vy-uy*vx)
    // ||v|| = (vx^2+vy^2+vz^2)^(1/2)
    float templength;
    crossProduct(&u, camera.vdir, camera.up);
    templength = vectorLength(u);
    u.x = u.x / templength; 
    u.y = u.y / templength; 
    u.z = u.z / templength; 

    crossProduct(&v, u, camera.vdir);
    templength = vectorLength(v);
    v.x = v.x / templength; 
    v.y = v.y / templength; 
    v.z = v.z / templength; 

    // ul = view_origin + d×n – (w/2)×u + (h/2)×v
    // ur = view_origin + d×n + (w/2)×u + (h/2)×v
    // ll = view_origin + d×n – (w/2)×u – (h/2)×v
    // lr = view_origin + d×n + (w/2)×u – (h/2)×v
    // w = 2*d*tan(1/2 * HFOV)
    // aspect_ratio = width / height
    // h = w / aspect_ratio
    vector_t tempV = v;
    vector_t tempU = u;
    float aspect_ratio = (float)camera.width / (float)camera.height;
    float tempAngle = camera.hfov * (3.14159265359 / 180);
    float window_width = 2 * camera.d * tanf(1.0/2.0 * tempAngle); 
    float window_height = window_width / aspect_ratio;
    vector_t n = camera.vdir;
    vectorMult(&n, 1 / vectorLength(n));
    vectorMult(&n, camera.d);
    

    // ul
    camera.ul = camera.eye;
    vectorMult(&tempU, -window_width * 0.5);    
    vectorMult(&tempV, window_height * 0.5);     
    vectorAddVector(&camera.ul, tempU);
    vectorAddVector(&camera.ul, tempV);
    vectorAddVector(&camera.ul, n);

    // Cleanup
    tempV = v;
    tempU = u;

    // ur
    camera.ur = camera.eye;
    vectorMult(&tempU, window_width/2.0);
    vectorMult(&tempV, window_height/2.0);
    vectorAddVector(&camera.ur, tempU);
    vectorAddVector(&camera.ur, tempV);
    vectorAddVector(&camera.ur, n);

    // Cleanup
    tempV = v;
    tempU = u;

    // ll
    camera.ll = camera.eye;
    vectorMult(&tempU, -window_width/2.0);
    vectorMult(&tempV, -window_height/2.0);
    vectorAddVector(&camera.ll, tempU);
    vectorAddVector(&camera.ll, tempV);
    vectorAddVector(&camera.ll, n);

    // Cleanup
    tempV = v;
    tempU = u;

    // lr
    camera.lr = camera.eye;
    vectorMult(&tempU, window_width/2.0);
    vectorMult(&tempV, -window_height/2.0);
    vectorAddVector(&camera.lr, tempU);
    vectorAddVector(&camera.lr, tempV);
    vectorAddVector(&camera.lr, n);

    // Gets the name for the output file
    // Assumes that the input file has at most 1 period

    token1 = strtok(argv[1], delim);
    strcat(outputFileName, token1);
    strcat(outputFileName, ".ppm");

    // Creates a new file and hands it off to imageWriter
    // Checks for errors
    outputFilePointer = fopen(outputFileName, "w");
    if (!outputFilePointer){
        printf("Failed to create file\n");
        return 1;
    }

    writerOutput = imageWriter(outputFilePointer);
    fclose(outputFilePointer);
    free(camera.image);

    // Checks for an error from imageWriter
    if (writerOutput){
        return 1;
    }

    return 0;
}