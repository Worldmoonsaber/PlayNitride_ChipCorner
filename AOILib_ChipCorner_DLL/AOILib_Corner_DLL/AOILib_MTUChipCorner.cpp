
#include "AOILib_MTUChipCorner.h"
#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> //mophorlogical operation
#include<opencv2/core.hpp>
#include <iostream>
#include <numeric>

using namespace cv;
using namespace std;





//void MTUkey_calcenter(thresP thresParm, ImgP imageParm, SettingP chipsetting, sizeTD target, unsigned int* imageIN,
//					unsigned int* imageOUT, unsigned char* imageGray, float boolResult[], float outputLEDX[], float outputLEDY[])
//{
//
//	
//	Mat rawimg, cropedRImg, gauBGR;
//	Mat Gimg, drawF2;
//
//	
//	Point IMGoffset=Point(0,0);
//
//	
//	
//
//	//output parameters::
//	Point2f crossCenter;
//	int boolflag = 0;
//
//	Mat image_input(imageParm.rows, imageParm.cols, CV_8UC4, &imageIN[0]); // THIS IS THE INPUT IMAGE, POINTER TO DATA			
//	image_input.copyTo(rawimg);
//
//	Mat image_output(1533, 1773, CV_8UC4, &imageOUT[0]);
//	Mat thres_output(1533, 1773, CV_8UC1, &imageGray[0]);
//
//	try
//	{
//		if (rawimg.empty())
//		{
//			boolflag = 8;
//			throw "something wrong::input image failure";
//		} //check if image is empty
//
//	} //try loop
//	catch (const char* message)
//	{
//
//		std::cout << "check catch state:: " << boolflag << endl;
//
//
//	}//catch loop
//
//	
//
//	if (boolflag == 0) //&& imageParm.Outputmode == 0
//	{
//		/*image with CROP  process :::*/
//		//Point piccenter;
//		//piccenter = find_piccenter(rawimg);
//		////std::cout << "pic center is ::" << piccenter.x << " , " << piccenter.y << endl;	
//		//IMGoffset.x = piccenter.x - int(imageParm.cols * 0.5);  //2736-600*0.5=2476
//		//IMGoffset.y = piccenter.y - int(imageParm.rows * 0.5);  //1824-600*0.5=1564
//		//Rect Cregion(IMGoffset.x, IMGoffset.y, imageParm.cols, imageParm.rows);
//		//cropedRImg = CropIMG(rawimg, Cregion);
//
//		///*///*image without CROP  process :::*/
//		//sizeParm.CsizeW = rawimg.size[0];
//		//sizeParm.CsizeH = sizeParm.CsizeW;
//		rawimg.copyTo(cropedRImg);
//
//        cvtColor(cropedRImg, cropedRImg, COLOR_RGB2GRAY);
//
//
//		//start to ISP//////////////////////////
//
//        //std::tie(boolflag, Gimg, crossCenter, drawF2) = ThresFiltering_1211(boolflag, rawimg, cropedRImg, target, thresParm, chipsetting);
//        //std::tie(boolflag, Gimg, crossCenter, drawF2) = ThresFiltering_1219(boolflag, rawimg, cropedRImg, target, thresParm, chipsetting);
//        if (imageParm.PICmode == 0)
//        {
//            std::tie(boolflag, Gimg, crossCenter, drawF2) = ThresFiltering_0613(boolflag, rawimg, cropedRImg, target, thresParm, chipsetting);
//        }
//        else if (imageParm.PICmode == 1)
//        {
//            std::tie(boolflag, Gimg, crossCenter, drawF2) = ThresFilteringCK_1219(boolflag, rawimg, cropedRImg, target, thresParm, chipsetting);
//        }
//        else
//        {
//            std::tie(boolflag, Gimg, crossCenter, drawF2) = ThresFiltering_0613(boolflag, rawimg, cropedRImg, target, thresParm, chipsetting);
//        }
//		
//		
//	}
//
//	std::cout << "check img state:: " << boolflag << endl;
//	std::cout << "check center is ::" << crossCenter << endl;
//
//	
//
//	/*  :::::::OUTPUT area:::::::  */
//	outputLEDX[0] = crossCenter.x ;
//	outputLEDY[0] = crossCenter.y ;
//	Gimg.copyTo(thres_output);
//	drawF2.copyTo(image_output);
//	boolResult[0] = boolflag;
//}

void MTUChipCorner_GetCorner(unsigned int* imageIN, ImgP imageParm, paramChipCorner Param, unsigned int* imageOUT, float boolResult[], float fPosX[], float fPosY[])
{
}
