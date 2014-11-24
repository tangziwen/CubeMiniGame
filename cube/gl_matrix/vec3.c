#include <stdlib.h>
#include <math.h>

#include "gl-matrix.h"

vec3_t vec3_create(vec3_t vec) {
    vec3_t dest = calloc(sizeof(float), 3);

    if (vec) {
        dest[0] = vec[0];
        dest[1] = vec[1];
        dest[2] = vec[2];
    } else {
        dest[0] = dest[1] = dest[2] = 0;
    }

    return dest;
}

vec3_t vec3_set(vec3_t vec, vec3_t dest) {
    dest[0] = vec[0];
    dest[1] = vec[1];
    dest[2] = vec[2];

    return dest;
}

vec3_t vec3_add(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] += vec2[0];
        vec[1] += vec2[1];
        vec[2] += vec2[2];
        return vec;
    }

    dest[0] = vec[0] + vec2[0];
    dest[1] = vec[1] + vec2[1];
    dest[2] = vec[2] + vec2[2];
    
    return dest;
}

vec3_t vec3_subtract(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] -= vec2[0];
        vec[1] -= vec2[1];
        vec[2] -= vec2[2];
        return vec;
    }

    dest[0] = vec[0] - vec2[0];
    dest[1] = vec[1] - vec2[1];
    dest[2] = vec[2] - vec2[2];
    return dest;
}

vec3_t vec3_multiply(vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= vec2[0];
        vec[1] *= vec2[1];
        vec[2] *= vec2[2];
        return vec;
    }

    dest[0] = vec[0] * vec2[0];
    dest[1] = vec[1] * vec2[1];
    dest[2] = vec[2] * vec2[2];
    return dest;
}

vec3_t vec3_negate(vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = -vec[0];
    dest[1] = -vec[1];
    dest[2] = -vec[2];
    return dest;
}

vec3_t vec3_scale(vec3_t vec, double val, vec3_t dest) {
    if (!dest || vec == dest) {
        vec[0] *= val;
        vec[1] *= val;
        vec[2] *= val;
        return vec;
    }

    dest[0] = vec[0] * val;
    dest[1] = vec[1] * val;
    dest[2] = vec[2] * val;
    return dest;
}

vec3_t vec3_normalize(vec3_t vec, vec3_t dest) {
    if (!dest) { dest = vec; }

    double x = vec[0], y = vec[1], z = vec[2],
        len = sqrt(x * x + y * y + z * z);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        return dest;
    } else if (len == 1) {
        dest[0] = x;
        dest[1] = y;
        dest[2] = z;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    return dest;
}

vec3_t vec3_cross (vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest) { dest = vec; }

    double x = vec[0], y = vec[1], z = vec[2],
        x2 = vec2[0], y2 = vec2[1], z2 = vec2[2];

    dest[0] = y * z2 - z * y2;
    dest[1] = z * x2 - x * z2;
    dest[2] = x * y2 - y * x2;
    return dest;
}

double vec3_length(vec3_t vec) {
    double x = vec[0], y = vec[1], z = vec[2];
    return sqrt(x * x + y * y + z * z);
}

double vec3_dot(vec3_t vec, vec3_t vec2) {
    return vec[0] * vec2[0] + vec[1] * vec2[1] + vec[2] * vec2[2];
}

vec3_t vec3_direction (vec3_t vec, vec3_t vec2, vec3_t dest) {
    if (!dest) { dest = vec; }

    double x = vec[0] - vec2[0],
        y = vec[1] - vec2[1],
        z = vec[2] - vec2[2],
        len = sqrt(x * x + y * y + z * z);

    if (!len) {
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        return dest;
    }

    len = 1 / len;
    dest[0] = x * len;
    dest[1] = y * len;
    dest[2] = z * len;
    return dest;
}

vec3_t vec3_lerp(vec3_t vec, vec3_t vec2, double lerp, vec3_t dest) {
    if (!dest) { dest = vec; }

    dest[0] = vec[0] + lerp * (vec2[0] - vec[0]);
    dest[1] = vec[1] + lerp * (vec2[1] - vec[1]);
    dest[2] = vec[2] + lerp * (vec2[2] - vec[2]);

    return dest;
}

double vec3_dist(vec3_t vec, vec3_t vec2) {
    double x = vec2[0] - vec[0],
        y = vec2[1] - vec[1],
        z = vec2[2] - vec[2];
        
    return sqrt(x*x + y*y + z*z);
}

vec3_t vec3_unproject(vec3_t vec, mat4_t view, mat4_t proj, vec4_t viewport, vec3_t dest) {
    if (!dest) { dest = vec; }

    mat4_t m = mat4_create(NULL);
    double *v = malloc(sizeof(double) * 4);
    
    v[0] = (vec[0] - viewport[0]) * 2.0 / viewport[2] - 1.0;
    v[1] = (vec[1] - viewport[1]) * 2.0 / viewport[3] - 1.0;
    v[2] = 2.0 * vec[2] - 1.0;
    v[3] = 1.0;
    
    mat4_multiply(proj, view, m);
    if(!mat4_inverse(m, NULL)) { return NULL; }
    
    mat4_multiplyVec4(m, v, NULL);
    if(v[3] == 0.0) { return NULL; }

    dest[0] = v[0] / v[3];
    dest[1] = v[1] / v[3];
    dest[2] = v[2] / v[3];
    
    return dest;
}
