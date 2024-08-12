#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

int main()
{
    Mat img,imgOut;

    //----待驗證 晶片是暗色 背景較亮的情境
    for (int i = 1; i < 35; i++)
    {
        string str = "C:\\Git\\Code\\ChipCorner\\SampleImage\\0806-"+to_string(i) + ".bmp";
        img = imread(str);
        cvtColor(img, img, COLOR_RGB2RGBA);

        param pm;
        pm.Parameters[0] = 1;//SelectMode  0:DARK 1:Light
        pm.Parameters[1] = 30;//Threshold Value 0~255

        pm.Parameters[2] = 24;//Chip longSide
        pm.Parameters[3] = 0.7;//Chip longSide Min Tolerance
        pm.Parameters[4] = 1.5;  //Chip longSide Max Tolerance
        pm.Parameters[5] = 15;//Chip ShortSide
        pm.Parameters[6] = 0.7;//Chip ShortSide Min Tolerance
        pm.Parameters[7] = 1.5;//Chip ShortSide Max Tolerance
        pm.Parameters[8] = -70;//Chip X Pitch
        pm.Parameters[9] = 70;//Chip Y Pitch
        pm.Parameters[10] = 500;// Edge Exclusion Distance

        Point ptCorner;
        int notFoundReason = 0;
        float angle = 0;

        auto t_start = std::chrono::high_resolution_clock::now();


        GetChipCorner(img, pm, notFoundReason, ptCorner, angle, imgOut);

        auto t_end = std::chrono::high_resolution_clock::now();
        auto elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

        if (notFoundReason != 9)
        {
            std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
            std::cout << "Error :: " << notFoundReason << endl;
        }

        std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
        std::cout << "Corner Point :: " << ptCorner << endl;

        std::cout << "Angle :: " << angle << endl;

        std::cout << "spend time :: " << elapsed_time_ms << endl;
        std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
        Mat imgResize;
        resize(imgOut, imgResize, Size(1000, 1000), 0, 0, INTER_LINEAR);

        if (notFoundReason != 9)
        {
            putText(imgResize, "notFoundReason : " + to_string(notFoundReason), Point(100, 100), 2, 2.0, Scalar(0, 0, 255), 2);
        }
        else
        {
            putText(imgResize, "OK", Point(100, 100), 2, 2.0, Scalar(0, 255, 0), 2);

            putText(imgResize, "Angle:"+ to_string(angle), Point(100, 300), 2, 2.0, Scalar(0, 255, 0), 2);
            putText(imgResize, "Corner Point : ("+to_string(ptCorner.x)+","+ to_string(ptCorner.y)+")", Point(100, 500), 2, 2.0, Scalar(0, 255, 0), 2);

        }

        imwrite(str + ".result.jpg", imgResize);
    }

    return 0;
}














