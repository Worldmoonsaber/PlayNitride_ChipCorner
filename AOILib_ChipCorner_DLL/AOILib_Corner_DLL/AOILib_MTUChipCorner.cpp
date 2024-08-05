
#include "AOILib_MTUchipcorner_V1.h"

using namespace cv;
using namespace std;







void MTUChipCorner_GetCorner(unsigned int* imageIN, ImgP imageParm, paramChipCorner Param, unsigned int* imageOUT, float boolResult[], float fPosX[], float fPosY[],float fAngle[])
{

	Point ptCorner;
	int notFoundReason = 0;
	float angle = 0;

	param pm;

	for(int i=0;i< sizeof(Param.Parameters)/sizeof(Param.Parameters[0]); i++)
		pm.Parameters[i] = Param.Parameters[i];

	// P.S: Laview 輸入影像格式為 RGBA
	Mat image_input(imageParm.rows, imageParm.cols, CV_8UC4, &imageIN[0]); // THIS IS THE INPUT IMAGE, POINTER TO DATA		
	Mat image_output(imageParm.rows, imageParm.cols, CV_8UC4, &imageOUT[0]);
	Mat image_output_Processing;//----用於作業 因為在演算法流程中可能會改到指標導致 imageOUT 沒有變化

	try
	{
		if (image_input.empty())
		{
			notFoundReason = 8;
			throw "something wrong::input image failure";
		} //check if image is empty

	} //try loop
	catch (const char* message)
	{
		std::cout << "check catch state:: " << notFoundReason << endl;
	}//catch loop

	if (notFoundReason == 0)
	{
		GetChipCorner(image_input, pm, notFoundReason, ptCorner, angle, image_output_Processing);
	}
	image_output_Processing.copyTo(image_output);
	image_output_Processing.release();
	image_input.release();
	std::cout << "check img state:: " << notFoundReason << endl;
	std::cout << "check center is ::" << ptCorner << endl;
	std::cout << "check angle is ::" << angle << endl;

	/*  :::::::OUTPUT area:::::::  */
	fPosX[0] = ptCorner.x;
	fPosY[0] = ptCorner.y;
	fAngle[0] = angle;
	boolResult[0] = notFoundReason;

}
