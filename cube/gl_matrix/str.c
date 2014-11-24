#include <stdlib.h>
#include <stdio.h>

#include "gl-matrix.h"

void vec3_str(vec3_t vec, char *buffer) {
    sprintf(buffer, "[%g, %g, %g]", vec[0], vec[1], vec[2]);
}

void mat3_str(mat3_t mat, char *buffer) {
    sprintf(buffer, "[%g, %g, %g, %g, %g, %g, %g, %g, %g]", mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8]);
}

void mat4_str(mat4_t mat, char *buffer) {
    sprintf(buffer, "[%g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g]",
        mat[0], mat[1], mat[2], mat[3], 
        mat[4], mat[5], mat[6], mat[7],
        mat[8], mat[9], mat[10], mat[11], 
        mat[12], mat[13], mat[14], mat[15]);
}

void quat_str(quat_t quat, char *buffer) {
    sprintf(buffer, "[%g, %g, %g, %g]", quat[0], quat[1], quat[2], quat[3]);
}
