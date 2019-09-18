#include "./mat4.h"

void scalar_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = x, m[ 2] = x, m[ 3] = x,
    m[ 4] = x, m[ 5] = x, m[ 6] = x, m[ 7] = x,
    m[ 8] = x, m[ 9] = x, m[10] = x, m[11] = x,
    m[12] = x, m[13] = x, m[14] = x, m[15] = x;
}

void diagonal_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = 0, m[ 2] = 0, m[ 3] = 0,
    m[ 4] = 0, m[ 5] = x, m[ 6] = 0, m[ 7] = 0,
    m[ 8] = 0, m[ 9] = 0, m[10] = x, m[11] = 0,
    m[12] = 0, m[13] = 0, m[14] = 0, m[15] = x;
}

void identity_mat4(mat4 m) {
    diagonal_mat4(m, 1.0);
}

void ortho4(mat4 dest, float left, float right, float bottom, float top) {
    identity_mat4(dest);
    set_mat4(dest, 0, 0, 2.0 / (right - left));
    set_mat4(dest, 1, 1, 2.0 / (top - bottom));
    set_mat4(dest, 0, 3, -(right + left) / (right - left));
    set_mat4(dest, 1, 3, -(top + bottom) / (top - bottom));
}
