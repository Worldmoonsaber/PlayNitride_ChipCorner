#pragma once

#include "../../ChipCorner/MTChipCorner_lib.h"
/*
key function:
key: version==ThresFiltering_1211
*/

typedef struct
{
	int PICmode; //用不到,沿用格式,方便別人串接
	int Outputmode; //用不到,沿用格式,方便別人串接 
	int cols; //1500
	int rows; //1500    
	double correctTheta;//用不到,沿用格式,方便別人串接
}ImgP;

typedef struct
{
	float Parameters[20];
}paramChipCorner;

__declspec(dllexport) void MTUChipCorner_GetCorner(unsigned int* imageIN, ImgP imageParm, paramChipCorner Param,
	unsigned int* imageOUT, float boolResult[], float fPosX[], float fPosY[], float fAngle[]);





