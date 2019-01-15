// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace utils {



// Cube Symmetries i.e. linear maps which map the standard basis to other standard vectors or their negatives.
/*
            ^
            |
            |/
           -*-->
           /|
          L

*/

int _cubeSymSign(int signs, int k) {
    if ((signs & (1 << k)) != 0) {
        return -1;
    }
    return 1;
}

glm::mat3 intToCubeSymmetry(int n) {
    int perm = n & (1 + 2 + 4);
    int signs = (n & (8+16+32)) >> 3;
    glm::mat3 ret(0.0f);

    int i0 = perm / 3;
    int i1 = (i0 + 1 + perm % 2) % 3;
    int i2 = 3 - i0 - i1;
    ret[0][i0] = _cubeSymSign(signs, 0);
    ret[1][i1] = _cubeSymSign(signs, 1);
    ret[1][i2] = _cubeSymSign(signs, 2);
    return ret;
}

int cubeSymmetryToInt(glm::mat3 mat) {
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
        if (glm::abs(mat[0][j]) > 0.5) {
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


const glm::mat3 i2cs[] = {
    intToCubeSymmetry( 0), intToCubeSymmetry( 1), intToCubeSymmetry( 2), intToCubeSymmetry( 3),
    intToCubeSymmetry( 4), intToCubeSymmetry( 5), intToCubeSymmetry( 6), intToCubeSymmetry( 7),
    intToCubeSymmetry( 8), intToCubeSymmetry( 9), intToCubeSymmetry(10), intToCubeSymmetry(11),
    intToCubeSymmetry(12), intToCubeSymmetry(13), intToCubeSymmetry(14), intToCubeSymmetry(15),
    intToCubeSymmetry(16), intToCubeSymmetry(17), intToCubeSymmetry(18), intToCubeSymmetry(19),
    intToCubeSymmetry(20), intToCubeSymmetry(21), intToCubeSymmetry(22), intToCubeSymmetry(23),
    intToCubeSymmetry(24), intToCubeSymmetry(25), intToCubeSymmetry(26), intToCubeSymmetry(27),
    intToCubeSymmetry(28), intToCubeSymmetry(29), intToCubeSymmetry(30), intToCubeSymmetry(31),
    intToCubeSymmetry(32), intToCubeSymmetry(33), intToCubeSymmetry(34), intToCubeSymmetry(35),
    intToCubeSymmetry(36), intToCubeSymmetry(37), intToCubeSymmetry(38), intToCubeSymmetry(39),
    intToCubeSymmetry(40), intToCubeSymmetry(41), intToCubeSymmetry(42), intToCubeSymmetry(46),
    intToCubeSymmetry(44), intToCubeSymmetry(45), intToCubeSymmetry(46), intToCubeSymmetry(47),
};

int cubeSymMult(int n, glm::mat3 mat) {
    return cubeSymmetryToInt(i2cs[n] * mat);
}
int cubeSymMult(glm::mat3 mat, int n) {
    return cubeSymmetryToInt(mat * i2cs[n]);
}
int cubeSymMult(glm::mat3 mat, glm::mat3 mat2) {
    return cubeSymmetryToInt(mat * mat2);
}
int rotLZ(int n, int k) {
    return cubeSymMult(n, glm::mat4_cast(glm::angleAxis(k * glm::radians(90.0f * k), glm::vec3(0, 0, 1))));
}



}
#endif // UTILS_H