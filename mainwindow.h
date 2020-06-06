#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimerEvent>
#include <QDateTime>
#include <QCloseEvent>
#include <QVector>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QListWidgetItem>

#include "camerathread.h"
#include "setcameradlg.h"
#include "setprojectordlg.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Mat m_matInitImg; // 初始空图片

    // 相机变量
    camerathread m_camerathd;   // 相机抓取线程

    // 设置对话框
    SetCameraDlg m_setcameradlg; // 相机参数设置
    SetProjectorDlg m_setprojectordlg; // 投影仪设置

    // 定时器
    int m_iTimerId;
    QDateTime m_timeStart;

    // 状态栏
    QLabel *m_labelRunTime;
    QLabel *m_labelSysTime;

    // 串口
    QSerialPort *m_pSerialPort;
    QString     m_strSendCommand;

    // 数据库
    QString m_dbUser;
    QString m_dbPasswd;
    QString m_dbName;
    QString m_dbTable;
    QVector<QVector<QString>> m_tableData;
    int     m_iSelectItem; // table中选择的条目
    QVector<Mat> m_vecUserImgs; // 当前条目的所有图片

    // 保存图片
    QString m_strSavePath;      // 保存路径
    QString m_strCurPersonID;   // 当前病人ID
    QString m_strPersonPath;    // 病人图片文件夹

    bool    m_bIsPersonInfo;    // 是否处于个人信息状态

    // 状态
    bool    m_bOpenProject;     // 打开投影仪
    bool    m_bOpenLaser;       // 打开激发光
    bool    m_bAutoGain;        // 自动增益

    // 多屏幕
    QVector<QRect> m_vecScreenRect; // 多屏大小
    int     m_iPrimaryScreen;   // 主屏

public:
    // 定时器函数
    void timerEvent(QTimerEvent *event);

    // 按键响应
    void keyPressEvent(QKeyEvent *event);

    // 相机操作
    bool OpenCamera();
    void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);

    void updateTimeShow();

    // 数据库操作
    bool linkDB();
    void queryPersonlist(QString strname);

    // 串口
    void openSerialPort();
    void closeSerialPort();
    void sendSerialData(QString strsend);

    void loadUserImgs(QString strPath);    
    void sendCommand(QString command, QString arg1 = "", QString arg2 = "");
    void detectMultiScreen();

    bool shutdownWindows();

public slots:
    void readSerialData();
    void updateHistoryList(QIcon icon, QString strname);
    void updateHistoryList_Capture(QIcon icon, QString strname);
    void updateProjectData();
    void controlMotor(bool bdirection);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButton_personinfo_clicked();
    void on_pushButton_focus_clicked();
    void on_pushButton_illbrower_clicked();
    void on_pushButton_saveperson_clicked();
    void on_pushButton_query_clicked();
    void on_pushButton_returnpage_clicked();
    void on_pushButton_deleteitem_clicked();
    void on_pushButton_exportdata_clicked();
    void on_pushButton_createperson_clicked();

    void on_tableWidget_personlist_doubleClicked(const QModelIndex &index);
    void on_stackedWidget_buttons_currentChanged(int arg1);

    void on_action_camerasetting_triggered();
    void on_action_settime_triggered();
    void on_action_calib_triggered();
    void on_action_setprojector_triggered();
    void on_action_datastat_triggered();
    void on_action_datamanage_triggered();

    void on_pushButton_historypre_clicked();
    void on_pushButton_historynext_clicked();

    void on_listWidget_historyimage_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_tableWidget_personlist_clicked(const QModelIndex &index);
    void on_listWidget_historyimage_currentRowChanged(int currentRow);

    void on_pushButton_calibYsub_clicked();
    void on_pushButton_calibXsub_clicked();
    void on_pushButton_calibXadd_clicked();
    void on_pushButton_calibYadd_clicked();

    void on_lineEdit_cutstartx_textChanged(const QString &arg1);
    void on_lineEdit_cutstarty_textChanged(const QString &arg1);

    void on_pushButton_autofocus_clicked();
    void on_pushButton_laseropen_clicked();
    void on_pushButton_laserclose_clicked();
    void on_pushButton_projectoropen_clicked();
    void on_pushButton_projectorclose_clicked();
    void on_pushButton_tuneadd_clicked();
    void on_pushButton_tunesub_clicked();

    void on_horizontalSlider_gain_valueChanged(int value);
    void on_horizontalSlider_bright_valueChanged(int value);

    void on_pushButton_startcalib_clicked();
    void on_pushButton_calibok_clicked();

    void on_lineEdit_name_textChanged(const QString &arg1);
    void on_comboBox_sex_currentIndexChanged(const QString &arg1);
    void on_lineEdit_checkpos_textChanged(const QString &arg1);
    void on_spinBox_age_valueChanged(int arg1);
    void on_dateEdit_date_dateChanged(const QDate &date);
    void on_lineEdit_checkroom_textChanged(const QString &arg1);
    void on_lineEdit_menzhenno_textChanged(const QString &arg1);
    void on_lineEdit_zhuyuanno_textChanged(const QString &arg1);
    void on_lineEdit_bingquno_textChanged(const QString &arg1);
    void on_lineEdit_chuangweino_textChanged(const QString &arg1);
    void on_lineEdit_tocheckdoctor_textChanged(const QString &arg1);
    void on_lineEdit_checkdoctor_textChanged(const QString &arg1);
    void on_textEdit_comment_textChanged();

    void on_action_close_triggered();

    void on_stackedWidget_currentChanged(int arg1);


    void on_pushButton_capture_clicked();

    void on_pushButton_whitemode_clicked();

    void on_pushButton_zoomin_clicked();

    void on_pushButton_zoomout_clicked();

    void on_lineEdit_scale_textChanged(const QString &arg1);

    void on_pushButton_projectorflip_clicked();

    void on_pushButton_projectorflipUD_clicked();

    void on_pushButton_inversemode_clicked();

    void on_pushButton_imgenhance_clicked();

    void on_pushButton_autogain_clicked();

    void on_horizontalSlider_FrameRate_valueChanged(int value);

    void on_action_8_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
