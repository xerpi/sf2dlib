#include <string.h>
#include "sf2d_private.h"

//stolen from staplebutt
void GPU_SetDummyTexEnv(u8 num)
{
	GPU_SetTexEnv(num,
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_REPLACE,
		GPU_REPLACE,
		0xFFFFFFFF);
}

void loadIdentity44(float *m)
{
	if (!m) return;
	memset(m, 0x00, 16*4);
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void multMatrix44(float *m1, float *m2, float *m)
{
	int i, j;
	for (i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			m[i+j*4] = (m1[0+j*4]*m2[i+0*4]) + (m1[1+j*4]*m2[i+1*4]) + (m1[2+j*4]*m2[i+2*4]) + (m1[3+j*4]*m2[i+3*4]);
		}
	}
}

void SetUniformMatrix(u32 startreg, float* m)
{
	float param[16];

	param[0x0]=m[3]; //w
	param[0x1]=m[2]; //z
	param[0x2]=m[1]; //y
	param[0x3]=m[0]; //x

	param[0x4]=m[7];
	param[0x5]=m[6];
	param[0x6]=m[5];
	param[0x7]=m[4];
	
	param[0x8]=m[11];
	param[0x9]=m[10];
	param[0xa]=m[9];
	param[0xb]=m[8];

	param[0xc]=m[15];
	param[0xd]=m[14];
	param[0xe]=m[13];
	param[0xf]=m[12];

	GPU_SetFloatUniform(GPU_VERTEX_SHADER, startreg, (u32*)param, 4);
}

void initOrthographicMatrix(float *m, float left, float right, float bottom, float top, float near, float far)
{
	/*  ______________________
	   |                      |
	   |                      |
	   |                      |
	   |                      |
	   |                      |
	   |______________________| ^
	                            | x
	                       <-----
	                         y
	*/
	
	//Mirror
	//right = 400-right;
	//left = 400-left;
	//top = 240-top;
	//bottom = 240-bottom;
	
	float mp[4*4];
	
	mp[0x0] = 2.0f/(right-left);
	mp[0x1] = 0.0f;
	mp[0x2] = 0.0f;
	mp[0x3] = -(right+left)/(right-left);

	mp[0x4] = 0.0f;
	mp[0x5] = 2.0f/(top-bottom);
	mp[0x6] = 0.0f;
	mp[0x7] = -(top+bottom)/(top-bottom);

	mp[0x8] = 0.0f;
	mp[0x9] = 0.0f;
	mp[0xA] = -2.0f/(far-near);
	mp[0xB] = (far+near)/(far-near);

	mp[0xC] = 0.0f;
	mp[0xD] = 0.0f;
	mp[0xE] = 0.0f;
	mp[0xF] = 1.0f;
	
	float mp2[4*4];
	loadIdentity44(mp2);
	mp2[0xA] = 0.5;
	mp2[0xB] = -0.5;

	//Convert Z [-1, 1] to [-1, 0] (PICA shiz)
	multMatrix44(mp2, mp, m);
	
	//rotateMatrixZ(m, M_PI/2, false);
}
