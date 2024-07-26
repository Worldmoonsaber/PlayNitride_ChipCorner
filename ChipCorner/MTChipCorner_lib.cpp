#include "MTChipCorner_lib.h"
#include "OpenCV_Extension_Tool.h"


void CaculateStd(vector<float> sample, float& avg, float& std)
{
    avg = 0;
    std = 0;
    float sum1 = 0, sum2 = 0;

    for (int i = 0; i < sample.size(); i++)
        sum1 += sample[i];

    avg = sum1 / sample.size();


    for (int i = 0; i < sample.size(); i++) {
        sum2 = sum2 + (sample[i] - avg) * (sample[i] - avg);
    }

    float var = (double)sum2 / sample.size();

    std = sqrt(var);

}

/// <summary>
/// 共線性分析 刪除掉不適合的點 (這個方法有點慢,待優化)
/// </summary>
/// <param name="possibleRegion">至少要有4個點以上才能輸出有意義的結果</param>
vector<BlobInfo> CollinearRegions(Mat FullSizeEmptyImg, vector<BlobInfo> possibleRegion)
{
    if (possibleRegion.size() < 4)
        return possibleRegion;

    //------------- 以下有效 但是很慢
    vector<BlobInfo> output;


    vector<vector<Point>> vContour;

    for (int i = 0; i < possibleRegion.size(); i++)
        vContour.push_back(possibleRegion[i].contour());

    for (int i = 0; i < vContour.size(); i++)
        drawContours(FullSizeEmptyImg, vContour, i, Scalar(255, 255, 255));

    //----避免破碎邊緣影響結果

    for (int i = 0; i < possibleRegion.size(); i++)
    {
        //最接近距離與Index
        float shortDist = 1000000;
        int shortIdx = -1;
        for (int j = 0; j < possibleRegion.size(); j++)
        {
            if (i != j)
            {
                float x_diff = possibleRegion[i].Center().x - possibleRegion[j].Center().x;
                float y_diff = possibleRegion[i].Center().y - possibleRegion[j].Center().y;

                float dist = sqrt(x_diff * x_diff + y_diff * y_diff); 

                if (dist < shortDist)
                {
                    shortDist = dist;
                    shortIdx = j;
                }

            }
        }

        Size2f sz;

        if (possibleRegion[i].minRectWidth() > possibleRegion[i].minRectHeight())
            sz = Size2f(shortDist,1);
        else
            sz = Size2f(1,shortDist);

        RotatedRect rect2_Redraw = RotatedRect(possibleRegion[i].Center(), sz, possibleRegion[i].Angle());
        cv::Point2f vertices2f_2[4];
        rect2_Redraw.points(vertices2f_2);
        cv::Point vertices[4];

        for (int i = 0; i < 4; ++i)
            vertices[i] = vertices2f_2[i];

        cv::line(FullSizeEmptyImg,
            vertices[0], vertices[3],
            Scalar(255, 255,255));
    }


    vector<BlobInfo> tmp = RegionPartition(FullSizeEmptyImg);

    int areaMax = -1;
    int idxMax = -1;

    for (int i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].Area() > areaMax)
        {
            idxMax = i;
            areaMax = tmp[i].Area();
        }
    }

    output.push_back(tmp[idxMax]);

    FullSizeEmptyImg.release();


    return output;
}


/// <summary>
/// 刪除角度方向與其他Region差異較大的點
/// </summary>
/// <param name="accept"></param>
/// <returns></returns>
vector<BlobInfo> OutlierAngleFilter(vector<BlobInfo> accept)
{
    vector<float> angles;
    float angleAvg = 0, angleStd = 0;

    for (int i = 0; i < accept.size(); i++)
    {
        float angle = accept[i].Angle();
        angles.push_back(angle);
    }

    CaculateStd(angles, angleAvg, angleStd);
    vector<BlobInfo> acceptLine;

    for (int i = 0; i < accept.size(); i++)
    {
        float angleDiff = abs(angleAvg - accept[i].Angle());

        if (angleDiff < angleStd)
            acceptLine.push_back(accept[i]);
        else if (angleDiff < 90 + angleStd && angleDiff >90 - angleStd)
            acceptLine.push_back(accept[i]);

    }

    accept.clear();
    return acceptLine;

}


/// <summary>
/// 同方向線段分類 (已刪除角度差較大者)
/// </summary>
/// <param name="input"></param>
/// <param name="output1">方向1</param>
/// <param name="output2">方向2</param>
void LineDirectionClassify(vector<BlobInfo> input, vector<BlobInfo>& output1, vector<BlobInfo>& output2)
{
    output1.clear();
    output2.clear();

    for (int i = 0; i < input.size(); i++)
    {
        if (input[i].minRectWidth() > input[i].minRectHeight())
            output1.push_back(input[i]);
        else
            output2.push_back(input[i]);
    }
}

void DrawFittingLine(Mat& ResultImg,Mat& Line_Img,RotatedRect MinRect,Size sz)
{
    RotatedRect rect1_Redraw = RotatedRect(MinRect.center, sz, MinRect.angle);
    cv::Point2f vertices2f_1[4];
    rect1_Redraw.points(vertices2f_1);

    cv::Point vertices[4];
    for (int i = 0; i < 4; ++i) {
        vertices[i] = vertices2f_1[i];
    }

    cv::fillConvexPoly(ResultImg,
        vertices,
        4,
        Scalar(0, 255, 0));

    cv::fillConvexPoly(Line_Img,
        vertices,
        4,
        Scalar(255, 255, 255));
}

/*
void GetPanelCorner(Mat src, paramCorner Param, Point& CornerPoint, int& notFoundReason, Mat& ResultImg)
{
    string strOut;

    vector<uchar*> vPtr;

    if (src.empty())
    {
        notFoundReason = 8;
        throw "Cannot load image.";
    }

    ResultImg = src.clone();

    Mat grayImg = Mat(src.size(), CV_8UC1);

    cvtColor(src, grayImg, COLOR_RGB2GRAY);

    if (ResultImg.channels()==1)
        cvtColor(ResultImg, ResultImg, COLOR_GRAY2RGB);

    blur(grayImg, grayImg, Size(5, 5));
    Mat avgImg, stdImg, ImgBinary, ImgBinaryEdge;
    meanStdDev(grayImg, avgImg, stdImg);
    double avg = avgImg.ptr<double>(0)[0];
    float  stddev = stdImg.ptr<float>(0)[0];
    Mat panel;

    threshold(grayImg, ImgBinary, 125, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(100, 100));
    cv::morphologyEx(ImgBinary, ImgBinary, MORPH_CLOSE, element, Point(-1, -1));

    cv::morphologyEx(ImgBinary, ImgBinaryEdge, MORPH_DILATE, element, Point(-1, -1));
    cv::morphologyEx(ImgBinary, ImgBinary, MORPH_ERODE, element, Point(-1, -1));

    ImgBinaryEdge = ImgBinaryEdge - ImgBinary;


    Canny(grayImg, panel, 100, 255);

    bitwise_and(ImgBinaryEdge, panel, panel);

    vector<BlobInfo> regions = RegionPartition(panel);
    vector<BlobInfo> accept;
    vector<vector<Point>> contour;

    for (int i = 0; i < regions.size(); i++)
    {
        if (regions[i].Area() > 100 && regions[i].AspectRatio() > 3 && regions[i].Ra()*1.5 > regions[i].contour().size())
        {
            accept.push_back(regions[i]);
            contour.push_back(regions[i].contour());
            //regions[i].Ra()*1.5 > regions[i].contour().size() 此條件是為了過濾掉 有很多毛邊的區域 (通常是 獨立的碎塊)
        }
    }

    if (accept.size() == 0)
    {
        avgImg.release();
        stdImg.release();
        panel.release();
        element.release();
        ImgBinary.release();
        grayImg.release();
        ImgBinaryEdge.release();

        notFoundReason = 3;
        throw "Can Not Find The Appropriate Edge, please check the image!";
    }

    vector<BlobInfo> acceptLine = OutlierAngleFilter(accept);

    accept.clear();
    contour.clear();

    for (int i = 0; i < acceptLine.size(); i++)
        contour.push_back(acceptLine[i].contour());

    vector<BlobInfo> Line1_Region;
    vector<BlobInfo> Line2_Region;
    LineDirectionClassify(acceptLine, Line1_Region, Line2_Region);

    if (Line1_Region.size() * Line2_Region.size() == 0)
    {
        avgImg.release();
        stdImg.release();
        panel.release();
        element.release();
        ImgBinary.release();
        grayImg.release();
        ImgBinaryEdge.release();
        notFoundReason = 2;
        throw "Can not Find 2 Edge to search Coner .Please Check if it exist a corner on image!";
    }


    Mat imgX;
    imgX = Mat(src.size(), CV_8UC1);
    Line1_Region = CollinearRegions(imgX, Line1_Region);
    imgX.release();

    imgX = Mat(src.size(), CV_8UC1);
    Line2_Region = CollinearRegions(imgX, Line2_Region);

    vector<Point> cLine1;
    vector<Point> cLine2;

    for (int i = 0; i < Line1_Region.size(); i++)
        for (int j = 0; j < Line1_Region[i].contour().size(); j++)
            cLine1.push_back(Line1_Region[i].contour()[j]);

    for (int i = 0; i < Line2_Region.size(); i++)
        for (int j = 0; j < Line2_Region[i].contour().size(); j++)
            cLine2.push_back(Line2_Region[i].contour()[j]);


    RotatedRect minrect1 = minAreaRect(cLine1);
    RotatedRect minrect2 = minAreaRect(cLine2);

    Size2f sz1 = Size2f(10000, 2);
    Size2f sz2 = Size2f(2, 10000);

    Mat Line1_Img, Line2_Img;
    Line1_Img = Mat(src.size(), CV_8UC1);
    Line2_Img = Mat(src.size(), CV_8UC1);

    DrawFittingLine(ResultImg, Line1_Img, minrect1, sz1);
    DrawFittingLine(ResultImg, Line2_Img, minrect2, sz2);

    bitwise_and(Line1_Img, Line2_Img, Line1_Img);

    vector<BlobInfo> vBlob = RegionPartition(Line1_Img); //預期區域會很小 不會有記憶體堆積問題

    if (vBlob.size() > 0)
    {
        CornerPoint = Point(vBlob[0].Center().x, vBlob[0].Center().y);
        cv::circle(ResultImg, CornerPoint, 100, Scalar(0, 0, 255), 5);
    }
    else
    {
        notFoundReason = 1;
        strOut = "Can Not Find Corner";
    }


#pragma region 釋放內容
    avgImg.release();
    stdImg.release();
    panel.release();
    element.release();
    ImgBinary.release();
    grayImg.release();
    imgX.release();
    ImgBinaryEdge.release();
    Line1_Img.release();
    Line2_Img.release();
#pragma endregion

}
*/
void GetChipCorner(Mat src, paramChipCorner Param, Point& CornerPoint, int& notFoundReason, Mat& ResultImg)
{
    if (src.empty())
    {
        notFoundReason = 8;
        throw "Cannot load image.";
    }
}
