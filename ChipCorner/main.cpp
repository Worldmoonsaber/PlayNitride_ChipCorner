#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

int main()
{
    Mat img,imgOut;
    img = imread("C:\\Git\\Code\\ChipCorner\\SampleImage\\0806-9.bmp");
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
    pm.Parameters[8] = 100;// Chip Pitch

    Point ptCorner;
    int notFoundReason = 0;
    float angle = 0;

    auto t_start = std::chrono::high_resolution_clock::now();

    try
    {
        GetChipCorner(img, pm, notFoundReason, ptCorner, angle, imgOut);
    }
    catch (Exception ex)
    {
        std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
        std::cout << "Error :: " << notFoundReason << endl;
    }



    auto t_end = std::chrono::high_resolution_clock::now();
    auto elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    std::cout << "Corner Point :: " << ptCorner << endl;

    std::cout << "Angle :: " << angle << endl;

    std::cout << "spend time :: " << elapsed_time_ms << endl;
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    Mat imgResize;
    resize(imgOut, imgResize, Size(500, 500),0,0,INTER_LINEAR);

    imshow("Result", imgResize);
    cv:waitKey(0);

    system("PAUSE");

    return 0;
}














