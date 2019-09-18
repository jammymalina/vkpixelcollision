#ifndef MATH_MAT_4_H
#define MATH_MAT_4_H

typedef float mat4[16];

#define MAT4_CLONE(m) {                                                        \
    m[0],  m[1],  m[2],  m[3],                                                 \
    m[4],  m[5],  m[6],  m[7],                                                 \
    m[8],  m[9],  m[10], m[11],                                                \
    m[12], m[13], m[14], m[15]                                                 \
}

#define get_mat4(dest, row, column) (dest[(column) * 4 + (row)])
#define set_mat4(dest, row, column, value)                                            \
    do {                                                                       \
        dest[(column) * 4 + (row)] = value;                                    \
    } while(0)

void scalar_mat4(mat4 m, float x);
void diagonal_mat4(mat4 m, float x);
void identity_mat4(mat4 m);

void ortho4(mat4 dest, float left, float right, float bottom, float top);

#endif
