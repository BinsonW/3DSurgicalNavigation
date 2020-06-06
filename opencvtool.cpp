#include <QDebug>
#include <QString>

#include "opencvtool.h"

OpenCVTool::OpenCVTool()
{

}

// 缩放图像
Mat OpenCVTool::MatResize(Mat mat, double dscale)
{
    Size dsize = Size(mat.cols*dscale, mat.rows*dscale);
    Mat image2;
    resize(mat, image2, dsize);
    return image2;
}

// 显示图片
void OpenCVTool::showImage(Mat img, QLabel* label)
{
    QImage qimg = Mat2QImage(img);
    showImage(qimg, label);
}

// 显示图片
void OpenCVTool::showImage(QImage qimg, QLabel* label)
{
    label->clear();
    label->setPixmap(QPixmap::fromImage(qimg));
    label->resize(label->pixmap()->size());
}

// 将Mat转化为QImage
QImage OpenCVTool::Mat2QImage(Mat& image)
{
    QImage img;

    if (image.channels() == 3) { // 彩色图
        cvtColor(image, image, COLOR_BGR2RGB);
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                     image.cols*image.channels(), QImage::Format_RGB888);
    } else if (image.channels() == 1) { // 灰度图
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                     image.step, QImage::Format_Indexed8);
    } else {
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                     image.cols*image.channels(), QImage::Format_RGB888);
    }

    return img;
}

// 将Mat转化为QImage
Mat OpenCVTool::QImageToMat(QImage &image)
{
    Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cvtColor(mat, mat, COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

// 得到灰度图的最大最小值
void OpenCVTool::calcImgMinMax(Mat matGray, double& dmin, double& dmax)
{
    minMaxIdx(matGray, &dmin, &dmax);
    qDebug()<<"min ="<<dmin<<", max ="<<dmax;
}

// 矩阵阈值分割
Mat OpenCVTool::anaThreSegment(Mat matIgray, double dmin, double dmax)
{
    int type; // 0：二值化；1：反二值化；2：大于阈值的设为阈值；3：大于阈值的保留，其他设为0；4：大于阈值的设置0
    Mat mask1, mask2, mask;

    // 大于dmin的设为1.0，否则为0
    type = 0;
    threshold(matIgray, mask1, dmin, 1.0, type); // mask1为float型，最大为1.0，最小为0.0

    // 小于dmax的设为1.0，否则为0
    type = 1;
    threshold(matIgray, mask2, dmax, 1.0, type);

    // 掩膜合并，dmin与dmax之间的为1，其他为0
    mask = mask1.mul(mask2);
    mask = mask>0; // 比较的结果为unit8型，false位置为0，true位置为255

    return mask;
}

// 计算矩阵的均值
double OpenCVTool::calcMatMean(Mat matIgray, Mat mask)
{
    Scalar mvalue;
    mvalue = mean(matIgray, mask);
    return mvalue.val[0];
}

// 画箭头
void OpenCVTool::drawArrow(Mat& img, Point pStart, Point pEnd, int len, int alpha,
                           Scalar& color, int thickness, int lineType)
{
    const double PI = 3.1415926;
    Point arrow;

    // 计算 θ 角
    double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
    line(img, pStart, pEnd, color, thickness, lineType);

    // 计算箭角边的另一端的端点位置（上面的还是下面的要看箭头的指向，也就是pStart和pEnd的位置）
    arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);
    arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);
    line(img, pEnd, arrow, color, thickness, lineType);
    arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);
    arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);
    line(img, pEnd, arrow, color, thickness, lineType);
}
