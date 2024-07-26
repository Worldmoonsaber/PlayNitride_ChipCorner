#pragma once

#include "../../ChipCorner/MTChipCorner_lib.h"
/*
key function:
key: version==ThresFiltering_1211
*/

typedef struct
{
	int cols; //1500
	int rows; //1500    
}ImgP;

__declspec(dllexport)  void MTUChipCorner_GetCorner(unsigned int* imageIN, ImgP imageParm, paramChipCorner Param,
	unsigned int* imageOUT, float boolResult[], float fPosX[], float fPosY[]);




