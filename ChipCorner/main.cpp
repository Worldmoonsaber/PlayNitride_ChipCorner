#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

int main()
{
    Mat img,result;
    img = imread("C:\\Git\\Code\\ChipCorner\\1.bmp");

    paramChipCorner pm;
    pm.Parameters[0] = 0;

    Point ptCorner;
    int notFoundReason = 0;

    auto t_start = std::chrono::high_resolution_clock::now();
    GetChipCorner(img, pm, ptCorner, notFoundReason, result);
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;

    std::cout << "calculate color-filter time is:: " << elapsed_time_ms << endl;
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    cv:waitKey(0);

    return 0;
}














