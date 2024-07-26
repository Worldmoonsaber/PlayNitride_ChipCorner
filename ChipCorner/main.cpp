#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

int main()
{
    Mat img,result, grayimg;
    img = imread("C:\\Git\\Code\\ChipCorner\\3.bmp");

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


    auto t_start = std::chrono::high_resolution_clock::now();
//    GetChipCorner(img, pm, ptCorner, notFoundReason, result);
    /*Parm 2. auto-thres filter */


    float ra= pm.Parameters[0];
    float ra_Min= pm.Parameters[1];
    float ra_Max= pm.Parameters[2];

    float rb= pm.Parameters[3];
    float rb_Min= pm.Parameters[4];
    float rb_Max= pm.Parameters[5];

    float fChip_Pitch = pm.Parameters[6];


    int adaptWsize = 101; //235
    int adaptKsize = 4;  //8
    vector<vector<Point>> _debugArea;


    vector <vector<Point>>contours;
    int thresfilter = THRESH_BINARY;

    Mat equalImg=Mat(img.size(),CV_8UC1);
    Mat debugImg = Mat(img.size(), CV_8UC1);

    if (img.channels() == 3)
        cvtColor(img, grayimg, COLOR_RGB2GRAY);
    else
        grayimg = img;

    equalizeHist(grayimg, equalImg);

    Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));

    cv::morphologyEx(equalImg, equalImg, MORPH_OPEN, element, Point(-1, -1));

    threshold(equalImg, result, 240, 255, THRESH_BINARY);


    //auto t_flag1 = std::chrono::high_resolution_clock::now();
    //double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_flag1 - t_start).count();
    //std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    //std::cout << "flag 1 spend time :: " << elapsed_time_ms << endl;

    float maxArea = ra * rb * max(ra_Max,rb_Max);
    float minArea = ra * rb * min(ra_Min,rb_Min);
    vector<BlobInfo> vChips = RegionPartition(result, maxArea, minArea);
    vector<BlobInfo> vChips_Comfirmed;
    vector<BlobInfo> vChips_NotSure;

    //auto t_flag15 = std::chrono::high_resolution_clock::now();
    //elapsed_time_ms = std::chrono::duration<double, std::milli>(t_flag15 - t_flag1).count();
    //std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    //std::cout << "flag 1.5 spend time :: " << elapsed_time_ms << endl;

    //for (int i = 0; i < vChips.size(); i++)
    //    _debugArea.push_back(vChips[i].Points());

    //for (int i = 0; i < _debugArea.size(); i++)
    //    drawContours(debugImg, _debugArea, i, Scalar(255, 255, 255));

    //_debugArea.clear();

    float _compactness_ideal=(2+ ra/rb+ rb / ra )/CV_PI;

    for (int i = 0; i < vChips.size(); i++)
    {
        if (vChips[i].Rectangularity() > 0.7)
        {
            vChips_Comfirmed.push_back(vChips[i]);
        }
        else if (vChips[i].Compactness() < _compactness_ideal * 1.2 && vChips[i].Compactness() > 0.8*_compactness_ideal)
            vChips_Comfirmed.push_back(vChips[i]);// 通常會找到 Chip 有所滑動的情況
        //
        //  Rectangularity 與 Compactness 可能會出現條件互斥的情況
        //
    }

    //auto t_flag2 = std::chrono::high_resolution_clock::now();
    //elapsed_time_ms = std::chrono::duration<double, std::milli>(t_flag2 - t_flag1).count();
    //std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    //std::cout << "flag 2 spend time :: " << elapsed_time_ms << endl;

    
    if (vChips_Comfirmed.size() < 3)
    {
        notFoundReason = 1;
        throw "CAN NOT FIND ENOUGH Chips On Image";
    }


    //equalImg = Mat(img.size(), CV_8UC1);

    // 測試
    vector<Point> _allArea;

    for (int i = 0; i < vChips_Comfirmed.size(); i++)
    {
      //  _debugArea.push_back(vChips_Comfirmed[i].Points());

       // for (int j = 0; j < vChips_Comfirmed[i].contour().size(); j++)
            
        _allArea.push_back(vChips_Comfirmed[i].Center());
    }

    
    for (int i = 0; i < _debugArea.size(); i++)
    {
        drawContours(grayimg, _debugArea, i, Scalar(255, 255, 255));
    }


    RotatedRect minrect = minAreaRect(_allArea);

    for (int i = 0; i < _debugArea.size(); i++)
        drawContours(grayimg, _debugArea, i, Scalar(255, 255, 255));
    

    cv::Point2f vertices2f_2[4];
    minrect.points(vertices2f_2);
    cv::Point vertices[4];

    for (int i = 0; i < 4; ++i)
        vertices[i] = vertices2f_2[i];

    cv::line(grayimg,
        vertices[0], vertices[1],
        Scalar(255, 255, 255));

    cv::line(grayimg,
        vertices[1], vertices[2],
        Scalar(255, 255, 255));

    cv::line(grayimg,
        vertices[2], vertices[3],
        Scalar(255, 255, 255));

    cv::line(grayimg,
        vertices[3], vertices[0],
        Scalar(255, 255, 255));
    

    int nMid = 0;
    float dist = INTMAX_MAX;

    float x_c = img.cols / 2;
    float y_c = img.rows / 2;

    for (int i = 0; i < 4; ++i)
    {
        float cur_dist = (x_c - vertices[i].x) * (x_c - vertices[i].x) + (y_c - vertices[i].y) * (y_c - vertices[i].y);
        
        if (cur_dist < dist)
        {
            dist = cur_dist;
            nMid = i;
        }

    }




    auto t_end = std::chrono::high_resolution_clock::now();
    auto elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;

    std::cout << "spend time :: " << elapsed_time_ms << endl;
    std::cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << endl;
    cv:waitKey(0);

    return 0;
}














