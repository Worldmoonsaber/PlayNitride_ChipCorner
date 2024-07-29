#pragma once
#pragma once


#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> //mophorlogical operation
#include<opencv2/core.hpp>
#include <iostream>
#include <numeric>


using namespace cv;
using namespace std;

typedef struct
{
	float Parameters[20];
}paramChipCorner;

void GetChipCorner(Mat src, paramChipCorner Param, int& notFoundReason, Point& CornerPoint, float& fAngleOutput, Mat& imgOut);