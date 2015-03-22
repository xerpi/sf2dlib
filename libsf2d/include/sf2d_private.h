#ifndef SF2D_PRIVATE_H
#define SF2D_PRIVATE_H

#include <3ds.h>

void GPU_SetDummyTexEnv(u8 num);

// Matrix operations

void matrix_copy(float *dst, const float *src);
void matrix_identity4x4(float *m);
void matrix_mult4x4(const float *src1, const float *src2, float *dst);
void matrix_rotate_z(float *m, float rad);
void matrix_swap_xy(float *m);

void matrix_init_orthographic(float *m, float left, float right, float bottom, float top, float near, float far);
void matrix_gpu_set_uniform(const float *m, u32 startreg);

unsigned int next_pow2(unsigned int v);

#endif
