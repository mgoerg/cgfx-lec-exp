// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GL/glew.h"

#include <string>

namespace utils {

void printGLerror(std::string text="GL Error: ");

int clampInt(int v, int lowerBound, int upperBound);


//Index 1==0b0001 => 0b1000
//Index 7==0b0111 => 0b1110
//etc
extern unsigned char lookup[16];

uint8_t reverse(uint8_t n);

// Detailed breakdown of the math
//  + lookup reverse of bottom nibble
//  |       + grab bottom nibble
//  |       |        + move bottom result into top nibble
//  |       |        |     + combine the bottom and top results 
//  |       |        |     | + lookup reverse of top nibble
//  |       |        |     | |       + grab top nibble
//  V       V        V     V V       V
// (lookup[n&0b1111] << 4) | lookup[n>>4]





// Cube Symmetries i.e. linear maps which map the standard basis to other standard vectors or their negatives.
/*
            ^
            |
            |/
           -*-->
           /|
          L

*/

int _cubeSymSign(int signs, int k);
glm::mat3 intToCubeSymmetry(int n);
int cubeSymmetryToInt(glm::mat3 mat);

extern const glm::mat3 i2cs[];

int cubeSymMult(int n, glm::mat3 mat);
int cubeSymMult(glm::mat3 mat, int n);
int cubeSymMult(glm::mat3 mat, glm::mat3 mat2);
int rotZ(int k);

float norm(glm::mat4 mat);

}
#endif // UTILS_H