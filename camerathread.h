#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QVector>
#include <QIcon>
#include <QList>
#include <QQueue>
#include <QDate>

#include "uc480.h"
#include "uc480_deprecated.h"
#include "opencvtool.h"

// 截取一帧图像的数据结构
struct ImageStruct
{
    Mat matImg;         // 抓取的图像
    Mat matImgH;        // 直方图均衡化的图像
    QString strName;    // 待保存文件的名字
    bool bIsSave;       // 是否已保存
};

class camerathread : public QThread
{
    Q_OBJECT
public:
    explicit camerathread(QObject *parent = 0);
    void run();
    ~camerathread();

public:
    // 线程相关变量
    bool m_bstop; // 停止线程

    // 相机变量
    HCAM	m_hG;			// 相机句柄
    HWND	m_hWnd;			// 图像显示句柄
    INT		m_nColorMode;	// 颜色模式：Y8/RGB16/RGB24/REG32
    INT		m_nBitsPerPixel;// 像素位数
    INT		m_nSizeX;		// 相机采集图像的宽
    INT		m_nSizeY;		// 相机采集图像的高
    INT		m_lMemoryId;	// 内存ID
    char*	m_pcImageMemory;// 内存指针

    // 参数
    bool    m_bModified;
    bool    m_bFRateModified; // 如果修改的是帧率
    UINT    m_nExposure;    // 曝光时间    
    double  m_dExpMin;      // 最小曝光时间
    double  m_dExpMax;      // 最大曝光时间
    UINT    m_nGain;        // 增益
    UINT    m_nwbalance;    // 白平衡
    UINT    m_nframerate;   // 帧率
    double  m_dFRateMin;    // 最小帧率
    double  m_dFRateMax;    // 最大帧率
    bool    m_bAutoExposure;
    bool    m_bAutoGain;
    bool    m_bAutoWbalance;

    // 投影
    QString m_strIniName;
    int     m_iProjectWidth;
    int     m_iProjectHeight;
    int     m_iStartX;
    int     m_iStartY;
    double  m_dBright;      // 投影亮度
    QRect   m_rectScreen;
    double  m_dscale;       // 投影缩放系数
    bool    m_bflip;        // 是否左右翻转
    bool    m_bflipUD;        // 是否上下翻转

    // 当前病人信息
    QString m_strName;
    QString m_strSex;
    QString m_strCheckpos;
    QString m_strAge;
    QDate m_dateCheck;
    QString m_strCheckRoom;
    QString m_strMenzhenno;
    QString m_strZhuyuanno;
    QString m_strBingquno;
    QString m_strChuangweino;
    QString m_strTocheckdoctor;
    QString m_strCheckdoctor;
    QString m_strComment;

    // 截图
    bool    m_bSaveImg;
    QVector<ImageStruct> m_vecSnapImgs;

    // 暂停采集
    bool    m_bPauseCapture;

    // 自动对焦
    int     m_iAutoFocus; // 0：关闭；1：开始；2：正在对焦
    double  m_dClearness;
    bool    m_bDirection;
    bool    m_bMotorFinish; // 电机运动完成
    int     m_iAdjustCount; // 调整次数
    QQueue<int> m_queueDirection; // 方向调整

    // 校准
    int     m_iCalibration; // 0：停止校准状态；1：校准状态；2：停止校准；-1：切换到校准界面
    Point   m_ptRectSta;
    Point   m_ptRectEnd;
    Size    m_sizeBoard; // 棋盘格板点数
    Mat     m_matProject;

    // 白光模式
    bool    m_bWhiteMode;
    Mat     m_matWhitePro;

    // 反色
    bool    m_bInverseGray;

    // 增强对比度
    bool    m_benhance;

public:
    void stop();

    bool OpenCamera();
    void CloseCamera();
    void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);

    void SnapImage();
    void SetHWND(HWND hWnd);
    void setSaveImg();

    void captureAImage(Mat &img);
    void saveImages(QString strSavePath);
    void calcProjectWH(int &iw, int &ih);
    Mat mergeGrayToGreen(Mat &project, Mat &img);
    void setAutoGain(bool bEnable);
    void setAutoExpoture(bool bEnable);
    void setAutoWbalance(bool bEnable);
    void modifyCamera();

    void performAutoFocus(Mat &img);
    void performAutoFocus_Sim(Mat &img);
    double calcImgClearness(Mat &img);
    void performCalibration(Mat &img);

    Mat makeProjectImg(Mat &img);
    double calcSampleTime(int iframerate);
    void SetScreenRect(QVector<QRect> &vecScreenRect);
    void saveProjectStart();
signals:
    void snapAImage(QIcon icon, QString strname);
    void controlMotor(bool bdirection);
    void updateCalibration();

public slots:
    void setExposure(int nexp);
    void setGain(int ngain);
    void setWhiteBalance(int ngain);
    void setFrameRate(int nframerate);
    void setProjectBright(double dbright);
};

#endif // CAMERATHREAD_H
