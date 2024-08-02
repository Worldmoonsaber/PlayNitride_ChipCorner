#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"

/// <summary>
/// 已測試 RGB RGBA GRAY 影像皆可以正常運算 輸出
/// </summary>
/// <param name="src"></param>
/// <param name="Param"></param>
/// <param name="notFoundReason"></param>
/// <param name="CornerPoint"></param>
/// <param name="fAngleOutput"></param>
/// <param name="imgOut"></param>
void GetChipCorner(Mat src, param Param, int& notFoundReason, Point& CornerPoint, float& fAngleOutput, Mat& imgOut)
{
    if (src.empty())
    {
        notFoundReason = 8;
        throw "Cannot load image.";
    }
    CornerPoint = Point(0, 0);
    Mat grayimg,result;
    Mat element = getStructuringElement(MORPH_RECT, Size(30, 30));
    notFoundReason = 0;

    float ra = Param.Parameters[2];
    float ra_Min = Param.Parameters[3];
    float ra_Max = Param.Parameters[4];

    float rb = Param.Parameters[5];
    float rb_Min = Param.Parameters[6];
    float rb_Max = Param.Parameters[7];

    float fChip_ExclusiveDistance = Param.Parameters[8];


    vector <vector<Point>>contours;

    int thresfilter = THRESH_BINARY;

    if (Param.Parameters[0] == 0)
        thresfilter = THRESH_BINARY_INV;
    else if (Param.Parameters[0] == 1)
        thresfilter = THRESH_BINARY;

    int thresholdVal = Param.Parameters[1];


    if (src.channels() == 3)
        cvtColor(src, grayimg, COLOR_RGB2GRAY);
    else if(src.channels() == 4)
        cvtColor(src, grayimg, COLOR_RGBA2GRAY);
    else
        grayimg = src;

    threshold(grayimg, result, thresholdVal, 255, thresfilter);

    float maxArea = ra * rb * ra_Max*rb_Max;
    float minArea = ra * rb *ra_Min*rb_Min;
    vector<BlobInfo> vChips = RegionPartition(result, maxArea, minArea);
    vector<BlobInfo> vChips_Comfirmed;
    vector<BlobInfo> vChips_NotSure;

    float _angle = 0;
    float _rectangularity = 0;
    float _bulkiness = 0;
    float _AspectRatio = 0;
   // vChips_Comfirmed = vChips;

    for (int i = 0; i < vChips.size(); i++)
    {
        _rectangularity += vChips[i].Rectangularity();
        _bulkiness+= vChips[i].Bulkiness();
        _AspectRatio += vChips[i].AspectRatio();
    }
    _rectangularity /= vChips.size();
    _bulkiness/= vChips.size();
    _AspectRatio/= vChips.size();
    
    for (int i = 0; i < vChips.size(); i++)
    {

        if (vChips[i].AspectRatio() > 1.5 * _AspectRatio || vChips[i].AspectRatio() < 0.8 * _AspectRatio)
            continue;

        if (abs(vChips[i].Bulkiness()- _bulkiness)>1)
            continue;

        if (abs(vChips[i].Rectangularity() - _rectangularity) > 0.1)
            continue;

    
        vChips_Comfirmed.push_back(vChips[i]);// 通常會找到 Chip 有所滑動的情況
    }
    
    //Mat debug=Mat(src.size(),CV_8UC1);

    //for (int i = 0; i < vChips_Comfirmed.size(); i++)
    //{
    //    for(int j=0;j< vChips_Comfirmed[i].Points().size();j++)
    //        debug.at<uchar>(vChips_Comfirmed[i].Points()[j].y, vChips_Comfirmed[i].Points()[j].x)=255;
    //}

    if (vChips_Comfirmed.size() < 5)
    {
        notFoundReason = 1;
        grayimg.release();
        result.release();
        element.release();

        throw "Can Not Find Enough Chips On Image. At least 5 Chips in Frame ...";
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
        float cur_dist = norm(Point2f(x_c, y_c) -(Point2f) vertices[i]);

        if (cur_dist < dist)
        {
            dist = cur_dist;
            nMid = i;
        }
    }

    CornerPoint = vertices[nMid];

    if (CornerPoint.x > fChip_ExclusiveDistance &&
        CornerPoint.x < src.cols - fChip_ExclusiveDistance &&
        CornerPoint.y> fChip_ExclusiveDistance &&
        CornerPoint.y < src.rows - fChip_ExclusiveDistance
        )
    {
        //OK 在適當範圍
    }
    else
    {
        notFoundReason = 2;
        grayimg.release();
        result.release();
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
    
    if (imgOut.channels() == 3)
    {
        cv::line(imgOut,
            SecondPoint, CornerPoint,
            Scalar(0, 255, 0));

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(0, 255, 0));

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(0, 0, 255));
    }
    else if (imgOut.channels() == 4)
    {
        cv::line(imgOut,
            SecondPoint, CornerPoint,
            Scalar(0, 255, 0,255));

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(0, 255, 0,255));

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(0, 0, 255,255));
    }
    else if (imgOut.channels() == 1)
    {
        cv::line(imgOut,
            SecondPoint, CornerPoint,
            Scalar(150, 150, 150, 150));

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(150, 150, 150, 150));

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(100, 100, 100, 100));

    }


    atanVal *= -1;

    fAngleOutput = atanVal;


    grayimg.release();
    result.release();
    element.release();
}
