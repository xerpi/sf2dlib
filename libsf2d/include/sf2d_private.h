#ifndef SF2D_PRIVATE_H
#define SF2D_PRIVATE_H

#include <3ds.h>

void GPU_SetDummyTexEnv(u8 num);

void loadIdentity44(float *m);
void multMatrix44(float *m1, float *m2, float *m);
void initOrthographicMatrix(float *m, float left, float right, float bottom, float top, float near, float far);
void SetUniformMatrix(u32 startreg, float *m);


#endif
