#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

void GetChipCorner(Mat src, paramChipCorner Param, int& notFoundReason, Point& CornerPoint, float& fAngleOutput, Mat& imgOut)
{
    if (src.empty())
    {
        notFoundReason = 8;
        throw "Cannot load image.";
    }
    CornerPoint = Point(0, 0);
    Mat grayimg,result;
    Mat equalImg = Mat(src.size(), CV_8UC1);
    Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));
    notFoundReason = 0;

    float ra = Param.Parameters[0];
    float ra_Min = Param.Parameters[1];
    float ra_Max = Param.Parameters[2];

    float rb = Param.Parameters[3];
    float rb_Min = Param.Parameters[4];
    float rb_Max = Param.Parameters[5];

    float fChip_Pitch = Param.Parameters[6];

    vector<vector<Point>> _debugArea;

    vector <vector<Point>>contours;
    int thresfilter = THRESH_BINARY;

    if (src.channels() == 3)
        cvtColor(src, grayimg, COLOR_RGB2GRAY);
    else
        grayimg = src;

    equalizeHist(grayimg, equalImg);
    cv::morphologyEx(equalImg, equalImg, MORPH_OPEN, element, Point(-1, -1));
    threshold(equalImg, result, 240, 255, THRESH_BINARY);

    float maxArea = ra * rb * max(ra_Max, rb_Max);
    float minArea = ra * rb * min(ra_Min, rb_Min);
    vector<BlobInfo> vChips = RegionPartition(result, maxArea, minArea);
    vector<BlobInfo> vChips_Comfirmed;
    vector<BlobInfo> vChips_NotSure;

    float _compactness_ideal = (2 + ra / rb + rb / ra) / CV_PI;

    for (int i = 0; i < vChips.size(); i++)
    {
        if (vChips[i].Rectangularity() > 0.7)
            vChips_Comfirmed.push_back(vChips[i]);
        else if (vChips[i].Compactness() < _compactness_ideal * 1.2 && vChips[i].Compactness() > 0.8 * _compactness_ideal)
            vChips_Comfirmed.push_back(vChips[i]);// 通常會找到 Chip 有所滑動的情況
    }

    if (vChips_Comfirmed.size() < 3)
    {
        notFoundReason = 1;
        grayimg.release();
        result.release();
        equalImg.release();
        element.release();

        throw "CAN NOT FIND ENOUGH Chips On Image";
    }

    // 測試
    vector<Point> _allArea;

    for (int i = 0; i < vChips_Comfirmed.size(); i++)
        _allArea.push_back(vChips_Comfirmed[i].Center());

    RotatedRect minrect = minAreaRect(_allArea);

    cv::Point2f vertices2f_2[4];
    minrect.points(vertices2f_2);
    cv::Point vertices[4];

    for (int i = 0; i < 4; ++i)
        vertices[i] = vertices2f_2[i];

    int nMid = 0;
    float dist = INTMAX_MAX;

    float x_c = src.cols / 2;
    float y_c = src.rows / 2;

    for (int i = 0; i < 4; ++i)
    {
        float cur_dist = (x_c - vertices[i].x) * (x_c - vertices[i].x) + (y_c - vertices[i].y) * (y_c - vertices[i].y);

        if (cur_dist < dist)
        {
            dist = cur_dist;
            nMid = i;
        }

    }

    CornerPoint = vertices[nMid];

    if (CornerPoint.x > fChip_Pitch &&
        CornerPoint.x < src.cols - fChip_Pitch &&
        CornerPoint.y> fChip_Pitch &&
        CornerPoint.y < src.rows - fChip_Pitch
        )
    {
        //OK 在適當範圍
    }
    else
    {
        notFoundReason = 2;
        grayimg.release();
        result.release();
        equalImg.release();
        element.release();

        throw "No Obivious Chip Corner Please Move The Panel";
    }

    int x_sample_level = -1;
    dist = INTMAX_MAX;

    for (int i = 0; i < 4; ++i)
    {
        if (i == nMid)
            continue;

        float cur_dist = (vertices[nMid].y - vertices[i].y) * (vertices[nMid].x - vertices[i].y);

        if (cur_dist < dist)
        {
            dist = cur_dist;
            x_sample_level = i;
        }

    }

    float xx = vertices[nMid].x - vertices[x_sample_level].x;
    float yy = vertices[nMid].y - vertices[x_sample_level].y;

    float atanVal = atan(yy / xx) * 180.0 / CV_PI;

    //定義
    //順時針為負
    //逆時針為正

    Point SecondPoint = vertices[x_sample_level];

    imgOut = src.clone();
    cv::line(imgOut,
        SecondPoint, CornerPoint,
        Scalar(0, 255, 0));

    cv::line(imgOut,
        Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
        Scalar(0, 255, 0));
    
    atanVal *= -1;

    cv::circle(imgOut,
        vertices[nMid],50,
        Scalar(0, 0, 255));

    fAngleOutput = atanVal;


    grayimg.release();
    result.release();
    equalImg.release();
    element.release();
}
