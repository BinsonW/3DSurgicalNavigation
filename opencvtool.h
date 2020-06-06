#ifndef OPENCVTOOL_H
#define OPENCVTOOL_H

#include <QLabel>

#include "opencv.hpp"

using namespace std;
using namespace cv;

class OpenCVTool
{
public:
    OpenCVTool();

    static Mat MatResize(Mat mat, double dscale);
    static void showImage(Mat img, QLabel *label);
    static void showImage(QImage qimg, QLabel *label);
    static QImage Mat2QImage(Mat &image);
    static void calcImgMinMax(Mat matGray, double &dmin, double &dmax);
    static Mat anaThreSegment(Mat matIgray, double dmin, double dmax);
    static double calcMatMean(Mat matIgray, Mat mask);
    static void drawArrow(Mat &img, Point pStart, Point pEnd, int len, int alpha, Scalar &color, int thickness, int lineType);
    static Mat QImageToMat(QImage &image);
};

#endif // OPENCVTOOL_H
