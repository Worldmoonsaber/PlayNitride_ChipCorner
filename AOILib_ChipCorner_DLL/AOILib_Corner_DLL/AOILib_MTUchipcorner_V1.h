#pragma once

#include "../../ChipCorner/MTChipCorner_lib.h"
/*
key function:
key: version==ThresFiltering_1211
*/

typedef struct
{
	int PICmode; //�Τ���,�u�ή榡,��K�O�H�걵
	int Outputmode; //�Τ���,�u�ή榡,��K�O�H�걵 
	int cols; //1500
	int rows; //1500    
	double correctTheta;//�Τ���,�u�ή榡,��K�O�H�걵
}ImgP;

typedef struct
{
	float Parameters[20];
}paramChipCorner;

__declspec(dllexport) void MTUChipCorner_GetCorner(unsigned int* imageIN, ImgP imageParm, paramChipCorner Param,
	unsigned int* imageOUT, float boolResult[], float fPosX[], float fPosY[], float fAngle[]);


/*
notFoundReason
0:�����L��
1:�L�k��쨬���ƶq��Chip�ѧOCorner,�нT�{�O�_�����Chip
2:�ثe��Ӧ�m Chip���� �L�k�Q�P�_�����I
8:�v���j�p���~
9:���`�˥X���I�è��o����
*/


