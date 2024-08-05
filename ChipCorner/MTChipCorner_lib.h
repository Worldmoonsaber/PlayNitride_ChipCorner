#pragma once
#pragma once


#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> //mophorlogical operation
#include<opencv2/core.hpp>


using namespace cv;
using namespace std;

typedef struct
{
	float Parameters[20];
}param;


void GetChipCorner(Mat src, param Param, int& notFoundReason, Point& CornerPoint, float& fAngleOutput, Mat& imgOut);