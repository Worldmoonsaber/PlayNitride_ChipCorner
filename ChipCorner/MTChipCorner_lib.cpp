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

    CornerPoint = Point(0, 0);
    Mat grayimg, tmp_img,result;
    notFoundReason = 0;

    float ra = Param.Parameters[2];
    float ra_Min = Param.Parameters[3];
    float ra_Max = Param.Parameters[4];

    float rb = Param.Parameters[5];
    float rb_Min = Param.Parameters[6];
    float rb_Max = Param.Parameters[7];


    float chip_x_pitch = Param.Parameters[8];
    float chip_y_pitch = Param.Parameters[9];

    float fChip_ExclusiveDistance = Param.Parameters[10];

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
    imgOut = grayimg.clone();

    Mat element = getStructuringElement(MORPH_RECT, Size(chip_x_pitch, chip_y_pitch));
    morphologyEx(result, tmp_img, MORPH_CLOSE, element, Point(-1, -1), 1);
    //  理論上 Chip區域跟

#pragma region 大面積標籤 &過濾 折衷辦法 未來實作到RegionPartition內部 來提升效率
    vector<vector<Point>> countour;
    cv::findContours(tmp_img, countour, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());

    vector<vector<Point>> BigRegionCountour;

    for (int i = 0; i < countour.size(); i++)
    {    
        int _XmaxBound=-1, _YmaxBound = -1, _XminBound=INT16_MAX, _YminBound= INT16_MAX;
    
        bool isAddNewElement;

        for (int j = 0; j < countour[i].size(); j++)
            {
                if (countour[i][j].x > _XmaxBound)
                    _XmaxBound = countour[i][j].x;

                if (countour[i][j].y > _YmaxBound)
                    _YmaxBound = countour[i][j].y;

                if (countour[i][j].x < _XminBound)
                    _XminBound = countour[i][j].x;

                if (countour[i][j].y < _YminBound)
                    _YminBound = countour[i][j].y;

                if (j > 20 && ((_XmaxBound - _XminBound) + 1 == tmp_img.size().width || (_YmaxBound - _YminBound) + 1 == tmp_img.size().height))
                {
                    isAddNewElement = true;
                    BigRegionCountour.push_back(countour[i]);//如果影像有拍到角點&背景影像 背景影像
                    break;
                }

            }

    }
    tmp_img.release();

    tmp_img = cv::Mat::zeros(result.size(), CV_8UC1);
    cv::fillPoly(tmp_img, BigRegionCountour, cv::Scalar(255));
    result = result - tmp_img;
    tmp_img.release();
#pragma endregion




    if (src.channels() == 3)
        cvtColor(imgOut, imgOut, COLOR_GRAY2RGB);
    else if (src.channels() == 4)
        cvtColor(imgOut, imgOut, COLOR_GRAY2RGBA);
    
    float maxArea = ra * rb * ra_Max*rb_Max;
    float minArea = ra * rb *ra_Min*rb_Min;

    vector<BlobInfo> vChips = RegionPartition(result, maxArea, minArea);






    vector<BlobInfo> vChips_Comfirmed;

    float _angle = 0;
    float _rectangularity = 0;
    float _bulkiness = 0;
    float _AspectRatio = 0;

    for (int i = 0; i < vChips.size(); i++)
    {
        //_angle+= vChips[i].Rectangularity();
        _rectangularity += vChips[i].Rectangularity();
        _bulkiness+= vChips[i].Bulkiness();
        _AspectRatio += vChips[i].AspectRatio();
    }

    //_angle/= vChips.size();
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

        //用角度過濾效果不佳
        //if (abs(vChips[i].Angle() - _angle) > 25)
        //    continue;

        vChips_Comfirmed.push_back(vChips[i]);// 通常會找到 Chip 有所滑動的情況
    }
    

    if (vChips_Comfirmed.size() < 8)
    {
        notFoundReason = 1;
        grayimg.release();
        result.release();

        for (int i = 0; i < vChips_Comfirmed.size(); i++)
        {
            if (imgOut.channels() == 3)
                cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(0, 0, 255), 1, 50, 5);
            else if (imgOut.channels() == 4)
                cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(0, 0, 255,255), 1, 50, 5);
            else if (imgOut.channels() == 1)
                cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(150,150,150), 1, 50, 5);
        }

        return;
        //throw "Can Not Find Enough Chips On Image. At least 5 Chips in Frame ...";
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
        
        for (int i = 0; i < 3; i++)
        {
            if (imgOut.channels() == 3)
            {
                cv::line(imgOut,
                    vertices[i], vertices[i+1],
                    Scalar(0, 0, 255), 5);
            }
            else if (imgOut.channels() == 4)
            {
                cv::line(imgOut,
                    vertices[i], vertices[i + 1],
                    Scalar(0, 0, 255, 255), 5);
            }
            else if (imgOut.channels() == 1)
            {
                cv::line(imgOut,
                    vertices[i], vertices[i + 1],
                    Scalar(100, 100, 100, 100), 5);
            }
        }

        if (imgOut.channels() == 3)
        {
            cv::line(imgOut,
                vertices[0], vertices[3],
                Scalar(0, 0, 255), 5);
        }
        else if (imgOut.channels() == 4)
        {
            cv::line(imgOut,
                vertices[0], vertices[3],
                Scalar(0, 0, 255, 255), 5);
        }
        else if (imgOut.channels() == 1)
        {
            cv::line(imgOut,
                vertices[0], vertices[3],
                Scalar(100, 100, 100, 100), 5);
        }


        grayimg.release();
        result.release();

        return;
        //throw "No Obivious Chip Corner Please Move The Panel";
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
            Scalar(0, 255, 0), 5);

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(0, 255, 0), 5);

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(0, 0, 255),5);
    }
    else if (imgOut.channels() == 4)
    {
        cv::line(imgOut,
            SecondPoint, CornerPoint,
            Scalar(0, 255, 0,255), 5);

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(0, 255, 0,255), 5);

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(0, 0, 255,255), 5);
    }
    else if (imgOut.channels() == 1)
    {
        cv::line(imgOut,
            SecondPoint, CornerPoint,
            Scalar(150, 150, 150, 150), 5);

        cv::line(imgOut,
            Point2f(SecondPoint.x, CornerPoint.y), Point2f(CornerPoint.x, CornerPoint.y),
            Scalar(150, 150, 150, 150), 5);

        cv::circle(imgOut,
            vertices[nMid], 50,
            Scalar(100, 100, 100, 100), 5);

    }

    //-----DEBUG
    for (int i = 0; i < vChips_Comfirmed.size(); i++)
    {
        if (imgOut.channels() == 3)
            cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(0, 0, 255), 1, 50, 5);
        else if (imgOut.channels() == 4)
            cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(0, 0, 255, 255), 1, 50, 5);
        else if (imgOut.channels() == 1)
            cv::drawMarker(imgOut, vChips_Comfirmed[i].Center(), Scalar(150, 150, 150), 1, 50, 5);
    }


    //--------------------------
    atanVal *= -1;

    fAngleOutput = atanVal;

    grayimg.release();
    result.release();
    notFoundReason = 9;

}


