#include "utils.h"
#include <iostream>


void utils::printGLerror(std::string text/*="GL Error: "*/) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << text << error << std::endl;
    }
}

int utils::clampInt(int v, int lowerBound, int upperBound) {
    if (v < lowerBound) return lowerBound;
    if (v > upperBound) return upperBound;
    return v;
}


//Index 1==0b0001 => 0b1000
//Index 7==0b0111 => 0b1110
//etc
unsigned char utils::lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

uint8_t utils::reverse(uint8_t n) {
   // Reverse the top and bottom nibble then swap them.
   return (lookup[n&0b1111] << 4) | lookup[n>>4];
}

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

int utils::_cubeSymSign(int signs, int k) {
    if ((signs & (1 << k)) != 0) {
        return -1;
    }
    return 1;
}

glm::mat3 utils::intToCubeSymmetry(int n) {
    int perm = n & (1 + 2 + 4);
    int signs = (n & (8+16+32)) >> 3;
    glm::mat3 ret(0.0f);

    int i0 = perm / 3;
    int i1 = (i0 + 1 + perm % 2) % 3;
    int i2 = 3 - i0 - i1;
    ret[0][i0] = utils::_cubeSymSign(signs, 0);
    ret[1][i1] = utils::_cubeSymSign(signs, 1);
    ret[1][i2] = utils::_cubeSymSign(signs, 2);
    return ret;
}

int utils::cubeSymmetryToInt(glm::mat3 mat) {
    int perm = 0;
    int signs = 0;
    int i, j;
    for (i = 0; i < 3; i++) {
        if (glm::abs(mat[0][i]) > 0.5) {
            if (mat[0][i] < 0) 
                signs |= 1;
            perm += 3 * i;
            break;
        }
    }
    for (j = 0; j < 3; j++) {
        if (glm::abs(mat[1][j]) > 0.5) {
            if ((j - i) % 3 == -1)
            perm += 1;
            if (mat[1][j] < 0) 
                signs |= 2;
            break;
        }
    }

    if (mat[2][3-i-j] < 0) 
        signs |= 4;
    return perm | (signs << 3);
}


const glm::mat3 utils::i2cs[] = {
    utils::intToCubeSymmetry( 0), utils::intToCubeSymmetry( 1), utils::intToCubeSymmetry( 2), utils::intToCubeSymmetry( 3),
    utils::intToCubeSymmetry( 4), utils::intToCubeSymmetry( 5), utils::intToCubeSymmetry( 6), utils::intToCubeSymmetry( 7),
    utils::intToCubeSymmetry( 8), utils::intToCubeSymmetry( 9), utils::intToCubeSymmetry(10), utils::intToCubeSymmetry(11),
    utils::intToCubeSymmetry(12), utils::intToCubeSymmetry(13), utils::intToCubeSymmetry(14), utils::intToCubeSymmetry(15),
    utils::intToCubeSymmetry(16), utils::intToCubeSymmetry(17), utils::intToCubeSymmetry(18), utils::intToCubeSymmetry(19),
    utils::intToCubeSymmetry(20), utils::intToCubeSymmetry(21), utils::intToCubeSymmetry(22), utils::intToCubeSymmetry(23),
    utils::intToCubeSymmetry(24), utils::intToCubeSymmetry(25), utils::intToCubeSymmetry(26), utils::intToCubeSymmetry(27),
    utils::intToCubeSymmetry(28), utils::intToCubeSymmetry(29), utils::intToCubeSymmetry(30), utils::intToCubeSymmetry(31),
    utils::intToCubeSymmetry(32), utils::intToCubeSymmetry(33), utils::intToCubeSymmetry(34), utils::intToCubeSymmetry(35),
    utils::intToCubeSymmetry(36), utils::intToCubeSymmetry(37), utils::intToCubeSymmetry(38), utils::intToCubeSymmetry(39),
    utils::intToCubeSymmetry(40), utils::intToCubeSymmetry(41), utils::intToCubeSymmetry(42), utils::intToCubeSymmetry(46),
    utils::intToCubeSymmetry(44), utils::intToCubeSymmetry(45), utils::intToCubeSymmetry(46), utils::intToCubeSymmetry(47),
};

int utils::cubeSymMult(int n, glm::mat3 mat) {
    return utils::cubeSymmetryToInt(utils::i2cs[n] * mat);
}
int utils::cubeSymMult(glm::mat3 mat, int n) {
    return utils::cubeSymmetryToInt(mat * utils::i2cs[n]);
}
int utils::cubeSymMult(glm::mat3 mat, glm::mat3 mat2) {
    return utils::cubeSymmetryToInt(mat * mat2);
}
int utils::rotZ(int k) {
    return utils::cubeSymmetryToInt(glm::mat3_cast(glm::angleAxis(glm::radians(90.0f * k), glm::vec3(0, 0, 1))));
}


float utils::norm(glm::mat4 mat) 
{
    float d = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            d += mat[i][j] * mat[i][j];
        }
    }
    return d;
}



