#ifndef TEXTURE_H
#define TEXTURE_H

#include "structs.h"

/*
 * Handles the texture lookup for all textured faces
 * 
 * Parameters:
 *   face - face that needs its texture looked up
 *   alpha - part of the coordinate system of the face
 *   beta - part of the coordinate system of the face
 *   gamma - part of the coordinate system of the face
 *   textures - array containing all of the textures
 * 
 * Returns:
 *   a color from the texture that corresponds to the specific location on
 *   the face
 */
vector_t faceTextureLookUp(face_t face, float alpha, float beta, float gamma, texture_t textures[]);

/*
 * Handles the texture lookup for all textured spheres
 * 
 * Parameters:
 *   sphere - sphere that needs its texture looked up
 *   N - the normal vector of the sphere at the intersected point
 *   textures - array containing all of the textures
 * 
 * Returns:
 *   a color from the texture that corresponds to the specific location on
 *   the sphere
 */
vector_t sphereTextureLookUp(sphere_t sphere, vector_t N, texture_t textures[]);

#endif // TEXTURE_H