#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

int main()
{
    Mat img,imgOut;
    img = imread("C:\\Git\\Code\\ChipCorner\\4.bmp");
    cvtColor(img, img, COLOR_RGB2RGBA);

    paramChipCorner pm;
       
    pm.Parameters[0] = 300;//Chip longSide
    pm.Parameters[1] = 0.5;//Chip longSide Min Tolerance
    pm.Parameters[2] = 2;  //Chip longSide Max Tolerance
    pm.Parameters[3] = 200;//Chip ShortSide
    pm.Parameters[4] = 0.5;//Chip ShortSide Min Tolerance
    pm.Parameters[5] = 2;//Chip ShortSide Max Tolerance
    pm.Parameters[6] = 1000;// Chip Pitch

    Point ptCorner;
    int notFoundReason = 0;
    float angle = 0;

    auto t_start = std::chrono::high_resolution_clock::now();

    GetChipCorner(img, pm, notFoundReason, ptCorner, angle, imgOut);

    auto t_end = std::chrono::high_resolution_clock::now();
    auto elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    std::cout << "Corner Point :: " << ptCorner << endl;

    std::cout << "Angle :: " << angle << endl;

    std::cout << "spend time :: " << elapsed_time_ms << endl;
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    cv:waitKey(0);

    return 0;
}














