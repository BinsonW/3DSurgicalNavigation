
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <vector>
#include <QSettings>

#include "camerathread.h"

camerathread::camerathread(QObject *parent) : QThread(parent)
{
    m_bstop = true;

    m_hG = 0;
    m_nSizeX = 1280;
    m_nSizeY = 1024;
    m_nBitsPerPixel = 8;
    m_nColorMode = IS_SET_CM_Y8; // 只要灰度图

    // 初始参数文件
    m_strIniName = "NavigData.ini";
    QSettings settings(m_strIniName, QSettings::IniFormat);

    // 相机参数
    m_bModified = true;
    m_bFRateModified = true;
    m_nExposure = settings.value("CameraPara/Exposure", 100).toInt();
    m_dExpMin = 0.00898246;
    m_dExpMax = 66.6209;
    m_nGain = settings.value("CameraPara/Gain", 100).toInt();
    m_nwbalance = settings.value("CameraPara/WBalance", 100).toInt();
    m_bAutoExposure = false;
    m_bAutoGain = false;
    m_bAutoWbalance = false;
    m_dFRateMax = 17;
    m_dFRateMin = 0.5;
    m_nframerate = settings.value("CameraPara/FrameRate", 100).toInt();

    // 投影参数
    m_dBright = 1.0;
    m_iProjectWidth = 1150;
    m_iProjectHeight = 640;
    m_rectScreen = QRect(0, 0, 0, 0);    
    m_iStartX = settings.value("Project/StartX", 0).toInt();
    m_iStartY = settings.value("Project/StartY", 0).toInt();
    m_dscale = settings.value("Project/Scale", 1).toFloat();
    m_bflip = settings.value("Project/Flip", false).toBool();
    m_bflipUD = settings.value("Project/FlipUD", false).toBool();

    m_bSaveImg = false;
    m_vecSnapImgs.clear();

    m_bPauseCapture = false;

    // 自动对焦
    m_iAutoFocus = 0;
    m_dClearness = 0;
    m_bDirection = true;
    m_bMotorFinish = false;
    m_iAdjustCount = 0;
    m_queueDirection.clear();

//    // 校准
    m_iCalibration = 0;
    m_ptRectSta = Point(0, 0);
    m_ptRectEnd = Point(0, 0);
    m_sizeBoard = Size(5, 3);    // 标定板上每行、列的角点数

//    // 白光模式
    m_bWhiteMode = false;

//    m_bInverseGray = false;
//    m_benhance = false;

//    m_strAge = "40";
}

camerathread::~camerathread()
{
    m_vecSnapImgs.clear();

    // 终止线程
    if(isRunning())
    {
        qDebug()<<"isRunning";
        stop();
        wait();
    }

    // 关闭相机
    CloseCamera();
    qDebug()<<"~camerathread";
}

// 截图
void camerathread::setSaveImg()
{    
    m_bSaveImg = true;
}

// 停止线程
void camerathread::stop()
{
    m_bstop = true;
}

// 线程函数
void camerathread::run()
{
    m_bstop = false;
    qDebug()<<"Start Camera.";
    while(!m_bstop)
    {
        if(m_bWhiteMode) // 白光模式
        {
            imshow("Project", m_matWhitePro);
            Sleep(500);
            continue; // 白光模式不再采集图像
        }

        if(m_hG != 0)
        {
            // 修改相机设置
            modifyCamera();

            if(!m_bPauseCapture)
            {
                // 抓取一帧
                INT iRet = is_FreezeVideo(m_hG, IS_WAIT);
                if(iRet == IS_SUCCESS)
                {
                    // 构造Mat
                    Mat img(m_nSizeY, m_nSizeX, CV_8UC1, m_pcImageMemory);

//                    // 保存图像
//                    if(m_bSaveImg)
//                    {
//                        m_bSaveImg = false;
//                        captureAImage(img);
//                    }

                    // 自动对焦
                    performAutoFocus_Sim(img);

//                    // 对比度增强
//                    if(m_benhance)
//                    {
//                        if(0)
//                            equalizeHist(img, img); // 直方图变换
//                        else
//                        {
//                            // Gamma矫正
//                            Mat X;
//                            img.convertTo(X, CV_32FC1); // 转为浮点数
//                            float gamma = 1/2.2;
//                            pow(X, gamma, X); // gamma矫正
//                            normalize(X, img, 0, 255, NORM_MINMAX, CV_8UC1); // 归一化并赋值回img
//                        }
//                    }

                    // 投影缩放
                    Mat imgp = OpenCVTool::MatResize(img, m_dscale);
                    // 投影翻转
                    if(m_bflip) // 左右翻转
                        flip(imgp, imgp, 1);
                    if(m_bflipUD) // 上下翻转
                        flip(imgp, imgp, 0);
                     //投影
                    if(m_iCalibration == -1) // 切换到校准界面
                        imshow("Project", m_matWhitePro);
                    else
                    {
                        Mat project = makeProjectImg(imgp);
                        Mat green = mergeGrayToGreen(project, imgp);
                        imshow("Project", green);
                    }

//                    // 校准
                    performCalibration(img); // 校准会改变图像

                    // 显示
                    is_RenderBitmap(m_hG, m_lMemoryId, m_hWnd, IS_RENDER_FIT_TO_WINDOW);
                }
                else // 中间拔掉USB，造成抓取失败
                    CloseCamera();
                //                Sleep(20);
            }
            else
            {
                // 显示之前抓取的
                is_RenderBitmap(m_hG, m_lMemoryId, m_hWnd, IS_RENDER_FIT_TO_WINDOW);
                Sleep(200);
            }
        }
        else
        {
            m_bSaveImg = false;
            OpenCamera();
            Sleep(500);
        }
    }
    qDebug()<<"Camera Over.";
}

// 修改了相机设置
void camerathread::modifyCamera()
{
    // 相机设置发生改变
    if(m_bModified)
    {
        m_bModified = false;

        if(m_bFRateModified)
        {
            m_bFRateModified = false;

            // 设置帧率
            double dfrate, dnewfrate;
            dfrate = (m_dFRateMax - m_dFRateMin)/100*m_nframerate+m_dFRateMin;
            is_SetFrameRate(m_hG, dfrate, &dnewfrate);
            //        qDebug("New Framerate = %f(%f)", dnewfrate, dfrate);

            // 相应调整曝光时间范围
            is_Exposure(m_hG, IS_EXPOSURE_CMD_GET_FINE_INCREMENT_RANGE_MIN,
                        (void*)&m_dExpMin, sizeof(m_dExpMin));
            is_Exposure(m_hG, IS_EXPOSURE_CMD_GET_FINE_INCREMENT_RANGE_MAX,
                        (void*)&m_dExpMax, sizeof(m_dExpMax));
        }

        // 设置曝光时间
        double dEnable = m_bAutoExposure;
        is_SetAutoParameter (m_hG, IS_SET_ENABLE_AUTO_SHUTTER, &dEnable, 0);
        if(!m_bAutoExposure)
        {
            double dexp;
            dexp = (m_dExpMax - m_dExpMin)/100*m_nExposure+m_dExpMin;
            is_Exposure(m_hG, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&dexp, sizeof(dexp));
        }

        // 设置增益
        dEnable = m_bAutoGain;
        is_SetAutoParameter (m_hG, IS_SET_ENABLE_AUTO_GAIN, &dEnable, 0);
        if(!m_bAutoGain)
            is_SetHardwareGain(m_hG, m_nGain, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);

        // 设置白平衡
        dEnable = m_bAutoWbalance;
        is_SetAutoParameter (m_hG, IS_SET_ENABLE_AUTO_WHITEBALANCE, &dEnable, 0);
        if(!m_bAutoWbalance)
            is_Blacklevel(m_hG, IS_BLACKLEVEL_CMD_SET_OFFSET, (void*)&m_nwbalance, sizeof(m_nwbalance));

        qDebug()<<"Exposure="<<m_nExposure<<", Gain="<<m_nGain<<", wBalance="<<m_nwbalance
               <<", FrameRate="<<m_nframerate;
    }

    // 自动状态下读取
    if(m_bAutoExposure) // 如果开着自动曝光，读取不断更新的曝光值
    {
        double dexp;
        is_Exposure(m_hG, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&dexp, sizeof(dexp));
        m_nExposure = (dexp-m_dExpMin)*100/(m_dExpMax - m_dExpMin);
    }
    if(m_bAutoWbalance) // 如果开着自动白平衡，读取不断更新的白平衡值
        is_Blacklevel(m_hG, IS_BLACKLEVEL_CMD_GET_OFFSET, (void*)&m_nwbalance, sizeof(m_nwbalance));
    if(m_bAutoGain) // 如果开着自动增益，读取不断更新的增益值
        m_nGain = is_SetHardwareGain(m_hG, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
}

// 校准
void camerathread::performCalibration(Mat &img)
{
    if(m_iCalibration == 1) // 校准状态
    {
        // 找格点
        vector<Point2f> image_points_buf;  // 缓存每幅图像上检测到的角点
        bool flag = findChessboardCorners(img, m_sizeBoard, image_points_buf, CALIB_CB_FAST_CHECK);
        qDebug()<<"Find "<<image_points_buf.size()<<" corners.";

        // 亚像素精确化
        // find4QuadCornerSubpix(image, image_points_buf, Size(5,5)); //对粗提取的角点进行精确化
        // cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));

        // 在图像上显示角点位置
        drawChessboardCorners(img, m_sizeBoard, image_points_buf, flag);

        // 如果找到所有点，则绘制框
        if(flag)
        {
            // 画框
            m_ptRectSta.x = round(image_points_buf[0].x);
            m_ptRectSta.y = round(image_points_buf[0].y);
            m_ptRectEnd.x = round(image_points_buf[image_points_buf.size()-1].x);
            m_ptRectEnd.y = round(image_points_buf[image_points_buf.size()-1].y);
            rectangle(img, m_ptRectSta, m_ptRectEnd, 255, 3);
        }
    }
    else if(m_iCalibration == 2) // 停止校准
    {
        m_iCalibration = 0;

        if(m_ptRectSta == Point(0, 0) && m_ptRectEnd == Point(0, 0))
            return;
        else
        {
            // 更新校准框
            m_iStartX = m_ptRectSta.x<m_ptRectEnd.x?m_ptRectSta.x:m_ptRectEnd.x;
            m_iStartY = m_ptRectSta.y<m_ptRectEnd.y?m_ptRectSta.y:m_ptRectEnd.y;
            int iwidth = abs(m_ptRectEnd.x - m_ptRectSta.x);
            int iheight = abs(m_ptRectEnd.y - m_ptRectSta.y);

            m_iStartX -= iwidth/(m_sizeBoard.width-1);
            m_iStartY -= iheight/(m_sizeBoard.height-1);
            if(m_iStartX < 0)
                m_iStartX = 0;
            if(m_iStartY < 0)
                m_iStartY = 0;

            // 发信号通知校准框更新
            emit updateCalibration();
        }
        qDebug()<<"calibration over.";
    }
}

// 保存投影起始值
void camerathread::saveProjectStart()
{
    // 保存投影参数
    QSettings settings(m_strIniName, QSettings::IniFormat);
    settings.setValue("Project/StartX", m_iStartX);
    settings.setValue("Project/StartY", m_iStartY);
    settings.setValue("Project/Scale", m_dscale);
    settings.setValue("Project/Flip", m_bflip);
    settings.setValue("Project/FlipUD", m_bflipUD);
    qDebug()<<"save project start value.";

    // 保存相机参数值
    settings.setValue("CameraPara/Exposure", m_nExposure);
    settings.setValue("CameraPara/Gain", m_nGain);
    settings.setValue("CameraPara/WBalance", m_nwbalance);
    settings.setValue("CameraPara/FrameRate", m_nframerate);
    qDebug()<<"save camera parameter value.";
}

// 自动对焦
void camerathread::performAutoFocus(Mat &img)
{
    if(m_iAutoFocus == 1)
    {
        qDebug()<<"Start autofocus.";
        // 开始对焦
        m_dClearness = calcImgClearness(img); // 记录起始清晰度
        m_iAutoFocus = 2;
        m_bDirection = true;
        m_bMotorFinish = false;
        m_iAdjustCount = 0;
        m_queueDirection.clear(); // 记录方向调整次数
        emit controlMotor(m_bDirection);
    }
    else if(m_iAutoFocus == 2 && m_bMotorFinish)
    {
        //        qDebug("Autofocus %d", m_iAdjustCount);
        // 对焦状态
        if(m_iAdjustCount > 30) // 达到最多调整次数，停止
            m_iAutoFocus = 0;
        else
        {
            double dClearness = calcImgClearness(img);
            qDebug()<<"Clearness: "<<dClearness;

            if(m_dClearness > dClearness)
                m_bDirection = !m_bDirection; // 反向
            m_dClearness = dClearness;
            m_bMotorFinish = false;
            m_iAdjustCount++;
            emit controlMotor(m_bDirection);

            // 记录调整次数
            m_queueDirection.enqueue(m_bDirection?1:-1);
            if(m_queueDirection.size() > 10) // 只记录10个
            {
                m_queueDirection.dequeue();

                // 来回振荡则停止调整
                double dsum = 0.0;
                for(int i = 0; i < m_queueDirection.size(); i++)
                    dsum += m_queueDirection.at(i);
                if(abs(dsum) < 2)
                    m_iAutoFocus = 0;
            }
        }
    }
}

// 仿真自动对焦
void camerathread::performAutoFocus_Sim(Mat &img)
{
    if(m_iAutoFocus == 1)
    {
        // 开始对焦
        m_iAutoFocus = 2;
        m_iAdjustCount = 1;
    }
    else if(m_iAutoFocus == 2)
    {
        // 对焦状态
        if(m_iAdjustCount > 5) // 达到最多调整次数，停止
            m_iAutoFocus = 0;
        else
        {
            // 图像锐化
            Mat kernel(3,3,CV_32F,Scalar(-1));
            kernel.at<float>(1,1) = 8+0.2*m_iAdjustCount;
            filter2D(img,img,img.depth(),kernel);

            m_iAdjustCount++;
        }
    }
}

// 图像清晰度评价函数
double camerathread::calcImgClearness(Mat &img)
{
    if(img.empty())
        return 0.0;

    // 转为灰度图
    Mat img1;
    if(img.channels() == 3)
        cvtColor(img, img1, COLOR_RGB2GRAY);
    else
        img1 = img;

    // 预处理
    int ih = img1.rows;
    int iw = img1.cols;
    img1 = Mat(img1, Rect(iw/4, ih/4, iw/2, ih/2)); // 截取中间部分

    // 计算Sobel算子
    Mat imgsobel;
    Sobel(img1, imgsobel, img1.depth(), 1, 0);

    // 计算
    Scalar meanValueImage;
    Scalar meanStdValueImage;
    meanStdDev(imgsobel, meanValueImage, meanStdValueImage);
    //    qDebug("Sobel value: %f, %f", meanValueImage.val[0], meanStdValueImage.val[0]);

    return meanStdValueImage.val[0];
}

// 计算实际投影宽高
void camerathread::calcProjectWH(int &iw, int &ih)
{
    if(m_iStartX + m_iProjectWidth > m_nSizeX-1)
        m_iStartX = m_nSizeX - m_iProjectWidth - 1;
    iw = m_iProjectWidth;

    if(m_iStartY + m_iProjectHeight > m_nSizeY-1)
        m_iStartY = m_nSizeY - m_iProjectHeight - 1;
    ih = m_iProjectHeight;
}

// 构造投影图
Mat camerathread::makeProjectImg(Mat &img)
{
    // 图片加白框
//    img.col(0) = 255;
//    img.col(img.cols-1) = 255;
//    img.row(0) = 255;
//    img.row(img.rows-1) = 255;
//    rectangle(img, Point(0, 0), Point(img.cols-1, img.rows-1), 255, 1);

    // 坐标
    int istartx = m_iStartX;
    int iendx = istartx + m_iProjectWidth - 1;
    int iwidth = max(iendx, img.cols-1) - min(istartx, 0) + 1;

    int istarty = m_iStartY;
    int iendy = istarty + m_iProjectHeight - 1;
    int iheight = max(iendy, img.rows-1) - min(istarty, 0) + 1;    

    // 构造底片（图像和投影的并区域）
    Mat ground = Mat::zeros(iheight, iwidth, CV_8UC1);
    Mat subground = ground(Rect(max(-istartx, 0), max(-istarty, 0), img.cols, img.rows));
    subground += img;

    // 截取
    Mat project = ground(Rect(max(istartx, 0), max(istarty, 0), m_iProjectWidth, m_iProjectHeight));

    return project;
}

// 灰度转化为绿色
Mat camerathread::mergeGrayToGreen(Mat &project, Mat &img)
{
    Mat zeros(project.size(), CV_8UC1);
    zeros.setTo(0);    

    // 投影区域加边框
//    int gap = 3;
//    zeros.col(0) = 255;
//    zeros.col(zeros.cols-gap) = 255;
//    zeros.row(0) = 255;
//    zeros.row(zeros.rows-gap) = 255;
//    project.col(0) = 255;
//    project.col(project.cols-gap) = 255;
//    project.row(0) = 255;
//    project.row(project.rows-gap) = 255;

    // 亮度调整
    Mat newproject;
    project.convertTo(newproject, project.type(), m_dBright);

    // 构造彩色图
    std::vector<Mat> bgr;
    Mat green;
    bgr.clear();
    bgr.push_back(zeros);
    bgr.push_back(newproject);
    bgr.push_back(zeros);
    merge(bgr, green);

    // 相机区域加白框
    rectangle(green,Point(max(-m_iStartX, 0), max(-m_iStartY, 0)),
              Point(min(-m_iStartX+img.cols-1, green.cols-1)-3, min(-m_iStartY+img.rows-1, green.rows-1)-3),
              Scalar(255,255,255), 1);

    // 画箭头
    int x = min(-m_iStartX+img.cols-1, green.cols-1);//*m_dscale;
    int y = min(-m_iStartY+img.rows-1, green.rows-1);//*m_dscale;
    Scalar colortemp=Scalar(255,255,255);
    OpenCVTool::drawArrow(green, Point(x-15, y-15), Point(x-100, y-15),
                          10, 30, colortemp, 1, LINE_8); // x
    OpenCVTool::drawArrow(green, Point(x-15, y-15), Point(x-15, y-100),
                          10, 30, colortemp, 1, LINE_8); // y

    return green;
}

// 设置显示框句柄
void camerathread::SetHWND(HWND hWnd)
{
    m_hWnd = hWnd;
}

// 设置第二屏幕的大小
void camerathread::SetScreenRect(QVector<QRect> &vecScreenRect)
{
    QRect rect;
    if(vecScreenRect.size() >= 2)
        rect = vecScreenRect[1]; // 多屏时，使用第二个屏的大小
    else
        rect = QRect(0, 0, 854, 480); // 单屏时，使用固定的大小

    // 乘以4/3，使投影大小与实际大小相同
    m_rectScreen = rect;
    m_iProjectWidth = rect.width();//*4/3;
    m_iProjectHeight = rect.height();//*4/3;

    // 构造校准图像
    QImage img(":/image/icons/checkerboard.png");
    Mat matimg = OpenCVTool::QImageToMat(img);

    int width, height;
    if(matimg.rows*rect.width() > matimg.cols*rect.height())
    {
        width = matimg.rows*rect.width()/rect.height();
        height = matimg.rows;
    }
    else
    {
        width = matimg.cols;
        height = matimg.cols*rect.height()/rect.width();
    }

    // 填补
    Mat project = Mat::zeros(height, width, CV_8UC1);
    Mat subproject = project(Rect(0, 0, matimg.cols, matimg.rows));
    //subproject += matimg;
    subproject.setTo(255);

    m_matWhitePro = project;
    m_matProject = mergeGrayToGreen(project, matimg);
}

// 打开相机
bool camerathread::OpenCamera()
{
    // 释放之前的图片内存
    if (m_hG != 0)
    {
        is_FreeImageMem(m_hG, m_pcImageMemory, m_lMemoryId);
        is_ExitCamera(m_hG);
    }

    // 初始化相机
    m_hG = (HCAM)0;
    INT iRet = is_InitCamera(&m_hG, NULL);
    if (iRet == IS_SUCCESS)
    {
        // 打开成功，获取相机信息
        GetMaxImageSize(&m_nSizeX, &m_nSizeY);
        //        is_GetColorDepth(m_hG, &m_nBitsPerPixel, &m_nColorMode);
        qDebug("(width, height) = (%d, %d), bits = %d, color = %d\n",
               m_nSizeX, m_nSizeY, m_nBitsPerPixel, m_nColorMode);

        // 获取相机参数
        is_Exposure(m_hG, IS_EXPOSURE_CMD_GET_FINE_INCREMENT_RANGE_MIN,
                    (void*)&m_dExpMin, sizeof(m_dExpMin));
        is_Exposure(m_hG, IS_EXPOSURE_CMD_GET_FINE_INCREMENT_RANGE_MAX,
                    (void*)&m_dExpMax, sizeof(m_dExpMax));
        qDebug()<<"Exposure(min, max) = "<<m_dExpMin<<", "<<m_dExpMax;

        double dminft, dmaxft, dinter;
        is_GetFrameTimeRange(m_hG, &dminft, &dmaxft, &dinter); // 得到采样时间范围
        m_dFRateMax = 1/dminft;
        m_dFRateMin = 1/dmaxft;
        qDebug("FrameRate: max = %f, min = %f", m_dFRateMax, m_dFRateMin);

        UINT nPixelClock;
        is_PixelClock(m_hG, IS_PIXELCLOCK_CMD_GET, (void*)&nPixelClock, sizeof(nPixelClock)); // 得到像素时钟
        qDebug("pixel clock = %d", nPixelClock);

        // 内存初始化
        is_AllocImageMem(m_hG,
                         m_nSizeX,
                         m_nSizeY,
                         m_nBitsPerPixel,
                         &m_pcImageMemory,
                         &m_lMemoryId);
        is_SetImageMem(m_hG, m_pcImageMemory, m_lMemoryId);	// 设置活动内存

        // 相机初始化
        is_SetColorMode(m_hG, m_nColorMode);
        is_SetImageSize(m_hG, m_nSizeX, m_nSizeY);
        is_SetDisplayMode(m_hG, IS_SET_DM_DIB);
    }
    else
        return false;

    return true;
}

// 关闭相机
void camerathread::CloseCamera()
{
    if (m_hG != 0)
    {
        is_FreeImageMem(m_hG, m_pcImageMemory, m_lMemoryId);
        is_ExitCamera(m_hG);
        m_hG = 0;
    }
}

// 获得相机图像尺寸
void camerathread::GetMaxImageSize(INT *pnSizeX, INT *pnSizeY)
{
    // 检查相机是否支持任意AOI
    INT nAOISupported = 0;
    BOOL bAOISupported = TRUE;
    if (is_ImageFormat(m_hG,
                       IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
                       (void*)&nAOISupported,
                       sizeof(nAOISupported)) == IS_SUCCESS)
        bAOISupported = (nAOISupported != 0);

    // 获取尺寸信息
    if (bAOISupported)
    {
        // 得到最大图像尺寸
        SENSORINFO sInfo;
        is_GetSensorInfo(m_hG, &sInfo);
        *pnSizeX = sInfo.nMaxWidth;
        *pnSizeY = sInfo.nMaxHeight;
    }
    else
    {
        // 得到当前格式的尺寸
        *pnSizeX = is_SetImageSize(m_hG, IS_GET_IMAGE_SIZE_X, 0);
        *pnSizeY = is_SetImageSize(m_hG, IS_GET_IMAGE_SIZE_Y, 0);
    }
}

// 抓取一帧图像保存
//void camerathread::SnapImage()
//{
//    if (m_hG != 0)
//    {
//        // 创建文件夹
//        QDir dir(m_strSavePath);
//        if(!dir.exists())
//            dir.mkpath(m_strSavePath);

//        // 生成文件名
//        QString strname = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
//        QString strfile = m_strSavePath + "//" + strname + ".jpg";
//        qDebug()<<"filename:"<<strfile;

//        // 保存图片
//        IMAGE_FILE_PARAMS ImageFileParams;
//        ImageFileParams.pnImageID = NULL;
//        ImageFileParams.ppcImageMem = NULL;
//        ImageFileParams.pwchFileName = (wchar_t *)strfile.utf16();;
//        ImageFileParams.nFileType = IS_IMG_JPG;
//        ImageFileParams.nQuality = 95;
//        is_ImageFile(m_hG, IS_IMAGE_FILE_CMD_SAVE, (void*)&ImageFileParams, sizeof(ImageFileParams));

//        // 历史记录中添加缩略图
//        Mat img(m_nSizeY, m_nSizeX, CV_8UC1, m_pcImageMemory);
//        Mat imgnew = img.clone();
//        //        m_vecSaveImgs.push_back(imgnew);
//        QImage qimg = OpenCVTool::Mat2QImage(imgnew);
//        emit snapAImage(QIcon(QPixmap::fromImage(qimg).scaled(QSize(150,150))), strname);
//    }
//}

// 抓取一帧图像
//void camerathread::captureAImage(Mat &img)
//{
//    if (m_hG != 0)
//    {
//        // 转为Mat
//        Mat imgnew = img.clone();
//        Mat imgH;
//        equalizeHist(img, imgH); // 直方图均衡化图像
//        ImageStruct imgdata;
//        imgdata.bIsSave = false;
//        imgdata.matImg = imgnew;
//        imgdata.matImgH = imgH;
//        imgdata.strName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz");
//        m_vecSnapImgs.push_back(imgdata);

//        // 历史记录中添加缩略图
//        Mat snapimgnew = img.clone();
//        QImage qimg = OpenCVTool::Mat2QImage(snapimgnew);
//        emit snapAImage(QIcon(QPixmap::fromImage(qimg).scaled(QSize(100,100))), imgdata.strName);
//    }
//}

// 保存图像
//void camerathread::saveImages(QString strSavePath)
//{
//    // 创建文件夹
//    QDir dir(strSavePath);
//    if(!dir.exists())
//        dir.mkpath(strSavePath);

//    // 保存
//    for(int i = 0; i < m_vecSnapImgs.size(); i++)
//    {
//        if(m_vecSnapImgs[i].bIsSave == false)
//        {
//            QString strfile = strSavePath + "//" + m_vecSnapImgs[i].strName + ".png";
//            if(imwrite(strfile.toStdString(), m_vecSnapImgs[i].matImg))
//                m_vecSnapImgs[i].bIsSave = true;

//            // 保存直方均衡化图像
//            QString strfileH = strSavePath + "//" + m_vecSnapImgs[i].strName + "_Histeq.png";
//            imwrite(strfileH.toStdString(), m_vecSnapImgs[i].matImgH);
//        }
//    }
//}

// 设置曝光时间
void camerathread::setExposure(int nexp)
{
    if(m_nExposure != nexp)
    {
        m_nExposure = nexp;
        m_bModified = true;
    }
}

// 设置相机增益
void camerathread::setGain(int ngain)
{
    if(m_nGain != ngain)
    {
        m_nGain = ngain;
        m_bModified = true;
    }
}

// 设置白平衡（黑电平）
void camerathread::setWhiteBalance(int nwbalance)
{
    if(m_nwbalance != nwbalance)
    {
        m_nwbalance = nwbalance;
        m_bModified = true;
    }
}

// 设置帧率
void camerathread::setFrameRate(int nframerate)
{
    if(m_nframerate != nframerate)
    {
        m_nframerate = nframerate;
        m_bModified = true;
        m_bFRateModified = true;
    }
}

// 启动自动增益
void camerathread::setAutoGain(bool bEnable)
{
    if(m_bAutoGain != bEnable)
    {
        m_bAutoGain = bEnable;
        m_bModified = true;
    }
}

// 启动自动曝光
void camerathread::setAutoExpoture(bool bEnable)
{
    if(m_bAutoExposure != bEnable)
    {
        m_bAutoExposure = bEnable;
        m_bModified = true;
    }
}

// 启动自动白平衡
void camerathread::setAutoWbalance(bool bEnable)
{
    if(m_bAutoWbalance != bEnable)
    {
        m_bAutoWbalance = bEnable;
        m_bModified = true;
    }
}

// 计算采样周期
double camerathread::calcSampleTime(int iframerate)
{
    double dfrate = (m_dFRateMax - m_dFRateMin)/100*iframerate+m_dFRateMin;
    if(dfrate == 0)
        dfrate = 1;
    return 1/dfrate;
}

// 设置投影亮度
void camerathread::setProjectBright(double dbright)
{
    m_dBright = dbright;
}
