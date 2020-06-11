#include <QDebug>
#include <QString>
#include <QTimer>
#include <QPixmap>
#include <QTableWidgetItem>
#include <QDialog>
#include <QDir>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "database.h"
#include "settimedlg.h"
//#include "datastatdlg.h"
//#include "datamanagedlg.h"

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    m_strCurPersonID = "";
//    m_strSavePath = "E://Navig//";
//    m_strPersonPath = m_strSavePath;
//    m_vecUserImgs.clear();
//    m_bIsPersonInfo = true;
    m_bOpenProject = false;
    m_bOpenLaser = false;
    m_bAutoGain = false;
    m_strSendCommand = "";

    //查找可用串口
    openSerialPort();

    // 多屏检测
    detectMultiScreen();

    // 相机对象初始化
    m_camerathd.SetHWND((HWND)ui->label_video->winId());
    if(m_camerathd.OpenCamera())
        qDebug()<<"Open Camera OK.";
    else
        qDebug()<<"Open Camera Error.";
    m_camerathd.SetScreenRect(m_vecScreenRect);
    m_matInitImg = Mat(m_camerathd.m_nSizeY,m_camerathd.m_nSizeX, CV_8UC1, Scalar::all(255));
    OpenCVTool::showImage(m_matInitImg, ui->label_video); // 初始显示一张图像，让程序自己调整label的大小
    m_setcameradlg.setCameraThd(&m_camerathd);
    if(!m_camerathd.isRunning())
        m_camerathd.start();

    // 投影初始化
    ui->label_originheight->setText(QString("%1").arg(m_camerathd.m_nSizeY));
    ui->label_originwidth->setText(QString("%1").arg(m_camerathd.m_nSizeX));
    updateProjectData();
    namedWindow("Project", WINDOW_NORMAL);
    if(m_vecScreenRect.size() > 1)
    {        
        moveWindow("Project", m_vecScreenRect[0].width(), 0);
        setWindowProperty("Project", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN); // 全屏显示
    }
    else
    {
        moveWindow("Project", -1100, 0);
        resizeWindow("Project", m_camerathd.m_iProjectWidth/2, m_camerathd.m_iProjectHeight/2);
    }

    //三维窗口初始化
    widget3d=new widget3D();
//    qDebug()<<"3dwindow initiate complete";
    //ui->horizontalLayout_2->addWidget(widget3d->container);
    ui->verticalLayout_14->removeWidget(ui->widget_2);
    ui->verticalLayout_14->addWidget(widget3d->container);
//    qDebug()<<"3dwindow add to layout complete";

    // 连接数据库
//    m_dbUser = "root";          // 用户名
//    m_dbPasswd = "123456";      // 密码
//    m_dbName = "lightImg.db";   // 文件名
//    m_dbTable = "data";         // 表名
//    DataBase::getInstance()->linkDB(m_dbUser, m_dbPasswd, m_dbName, m_dbTable);

    // 开启定时器
//    m_iTimerId = startTimer(33);
    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(grab3dscene()));
    timer->start(0);


    // 设置listwidget
//    ui->listWidget_historyimage->setSpacing(10);
//    ui->listWidget_historyimage->setIconSize(QSize(150,150));
//    connect(&m_camerathd, SIGNAL(snapAImage(QIcon, QString)), this, SLOT(updateHistoryList_Capture(QIcon, QString)));

    // 状态栏配置
    m_labelRunTime = new QLabel; // 运行时间
    m_labelRunTime->setMinimumSize(m_labelRunTime->sizeHint());
    m_labelRunTime->setAlignment(Qt::AlignLeft);
    statusBar()->addWidget(m_labelRunTime);

    m_labelSysTime = new QLabel("Ready1", this); // 系统时间
    statusBar()->addPermanentWidget(m_labelSysTime);

    statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 设置子控件border为0
    statusBar()->setSizeGripEnabled(false); // 不显示右边的大小控制点

    // 时间显示
    m_timeStart = QDateTime::currentDateTime(); // 获取系统现在的时间
    updateTimeShow(); // 更新时间

    // 其他初始化
//    m_tableData.clear();
//    m_iSelectItem = -1;
    //ui->pushButton_null->hide();
    connect(&m_camerathd, SIGNAL(controlMotor(bool)), this, SLOT(controlMotor(bool)));
    connect(&m_camerathd, SIGNAL(updateCalibration()), this, SLOT(updateProjectData()));
    connect(&m_setprojectordlg, SIGNAL(changeProjectBright(double)), &m_camerathd, SLOT(setProjectBright(double)));

    QSettings settings("NavigData.ini", QSettings::IniFormat);
    ui->horizontalSlider_gain->setValue(settings.value("CameraPara/SliderGain", 0).toInt());
    ui->horizontalSlider_bright->setValue(settings.value("Project/SliderBright", 0).toInt());

    // 初始状态
//    on_pushButton_createperson_clicked(); // 新建一个病人
//    ui->stackedWidget->setCurrentIndex(0);
//    ui->stackedWidget_buttons->setCurrentIndex(0);
//    ui->tabWidget_show->setCurrentIndex(0);

    // 向串口发启动命令
    sendCommand("START");
    Sleep(100); // 发送启动命令后，立即发送下面命令是否可靠？增加点延时
    sendCommand("LEDPW", "O"); // 开780
    sendCommand("LDDPW", "S"); // 关850
}

MainWindow::~MainWindow()
{
    // 关闭串口
    closeSerialPort();
    widget3d->container->close();
    delete widget3d;
    delete ui;
}

// 定时器响应函数
void MainWindow::timerEvent( QTimerEvent *event )
{
    // 更新时间显示
    updateTimeShow();
}

// 按键响应函数
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(!event->isAutoRepeat()) // 不要重复
    {
        if(event->modifiers() == Qt::ShiftModifier)
        {
            switch(event->key())
            {
            case Qt::Key_F3: // F13，暂停
                qDebug()<<"key press F13.";
                m_camerathd.m_bPauseCapture = !m_camerathd.m_bPauseCapture;
                break;
            case Qt::Key_F4: // F14，增益加
                qDebug()<<"key press F14.";
                ui->horizontalSlider_gain->setValue(ui->horizontalSlider_gain->value()+1);
                break;
            case Qt::Key_F5: // F15，增益减
                qDebug()<<"key press F15.";
                ui->horizontalSlider_gain->setValue(ui->horizontalSlider_gain->value()-1);
                break;
            case Qt::Key_F6: // F16，亮度加
                qDebug()<<"key press F16.";
                ui->horizontalSlider_bright->setValue(ui->horizontalSlider_bright->value()+1);
                break;
            case Qt::Key_F7: // F17，亮度减
                qDebug()<<"key press F17.";
                ui->horizontalSlider_bright->setValue(ui->horizontalSlider_bright->value()-1);
                break;
            case Qt::Key_F8: // F18，打开关闭投影光
                qDebug()<<"key press F18.";
                m_bOpenProject = !m_bOpenProject;
                if(m_bOpenProject)
                    sendCommand("PRJCT", "O");
                else
                    sendCommand("PRJCT", "S");
                break;
            case Qt::Key_F9: // F19，打开关闭激发光
                qDebug()<<"key press F19.";
                m_bOpenLaser = !m_bOpenLaser;
                if(m_bOpenLaser)
                    on_pushButton_laseropen_clicked();
                else
                    on_pushButton_laserclose_clicked();
                break;
            }
        }
        else if(event->modifiers() == Qt::ControlModifier)
        {
            switch(event->key())
            {
            case Qt::Key_F1: // F21，焦距微调
                qDebug()<<"key press F21.";
                //sendCommand("STEPM", "F");
                ui->horizontalSlider_gain->setValue(ui->horizontalSlider_gain->value()+1);
                break;
            case Qt::Key_F2: // F22，焦距微调
                qDebug()<<"key press F22.";
                //sendCommand("STEPM", "R");
                ui->horizontalSlider_gain->setValue(ui->horizontalSlider_gain->value()-1);
                break;
            case Qt::Key_F3: // F23，自动对焦
                qDebug()<<"key press F23.";
                //on_pushButton_autofocus_clicked();
                on_pushButton_autogain_clicked();
                break;
            case Qt::Key_F4: // F24，系统保留
                qDebug()<<"key press F24.";
                break;
            case Qt::Key_F5: // F25，关闭系统
                qDebug()<<"key press F25.";
                break;
            }
        }
    }
}

// 更新显示时间
void MainWindow::updateTimeShow()
{
    // 获取系统当前时间
    QDateTime curtime = QDateTime::currentDateTime();

    // 显示系统时间
    QString str = curtime.toString("系统时间: yyyy-MM-dd hh:mm:ss ");
    m_labelSysTime->setText(str);

    // 显示运行时间
    int imsec = m_timeStart.msecsTo(curtime);
    QTime zerotime(0, 0);
    QTime runtime = zerotime.addMSecs(imsec);
    m_labelRunTime->setText(runtime.toString(" 运行时间: hh:mm:ss  "));
}

// 关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭电源
    //sendCommand("PWROF");

    // 删除定时器
//    if(m_iTimerId != 0)
//        killTimer(m_iTimerId);

    // 关闭相机
    m_camerathd.saveProjectStart(); // 保存结果
    if(m_camerathd.isRunning())
        m_camerathd.stop();

    // 保存参数到ini文件
    QSettings settings("NavigData.ini", QSettings::IniFormat);
    settings.setValue("CameraPara/SliderGain", ui->horizontalSlider_gain->value());
    settings.setValue("Project/SliderBright", ui->horizontalSlider_bright->value());

    qDebug()<<"Program Over.";
}

// 个人信息选项卡
//void MainWindow::on_pushButton_personinfo_clicked()
//{
//    // 从其他界面切换过来，加载之前的病人信息
//    ui->lineEdit_name->setText(m_camerathd.m_strName);
//    ui->comboBox_sex->setCurrentText(m_camerathd.m_strSex);
//    ui->lineEdit_checkpos->setText(m_camerathd.m_strCheckpos);
//    ui->spinBox_age->setValue(m_camerathd.m_strAge.toInt());
//    ui->dateEdit_date->setDate(m_camerathd.m_dateCheck);
//    ui->lineEdit_checkroom->setText(m_camerathd.m_strCheckRoom);
//    ui->lineEdit_menzhenno->setText(m_camerathd.m_strMenzhenno);
//    ui->lineEdit_zhuyuanno->setText(m_camerathd.m_strZhuyuanno);
//    ui->lineEdit_bingquno->setText(m_camerathd.m_strBingquno);
//    ui->lineEdit_chuangweino->setText(m_camerathd.m_strChuangweino);
//    ui->lineEdit_tocheckdoctor->setText(m_camerathd.m_strTocheckdoctor);
//    ui->lineEdit_checkdoctor->setText(m_camerathd.m_strCheckdoctor);
//    ui->textEdit_comment->setText(m_camerathd.m_strComment);

//    // 从病案浏览回到个人信息的时候，加载已拍摄的图像的缩略图
//    m_vecUserImgs.clear();
//    ui->listWidget_historyimage->clear();
//    for(int i = 0; i < m_camerathd.m_vecSnapImgs.size(); i++)
//    {
//        Mat imgnew = m_camerathd.m_vecSnapImgs[i].matImg.clone();
//        QImage qimg = OpenCVTool::Mat2QImage(imgnew);
//        updateHistoryList(QIcon(QPixmap::fromImage(qimg).scaled(QSize(100,100))),
//                          m_camerathd.m_vecSnapImgs[i].strName);
//    }

//    m_bIsPersonInfo = true;
//    ui->stackedWidget->setCurrentIndex(0);
//    ui->stackedWidget_buttons->setCurrentIndex(0);
//    ui->tabWidget_show->setCurrentIndex(0);
//}

// 对焦选项卡
void MainWindow::on_pushButton_focus_clicked()
{
    qDebug()<<"focus button";
    ui->stackedWidget->setCurrentIndex(1);
}

// 运行状态选项卡
void MainWindow::on_pushButton_states_clicked()
{
    qDebug()<<"focus button";
    ui->stackedWidget->setCurrentIndex(0);
}

// 模型选项卡
void MainWindow::on_pushButton_modelbrower_clicked()
{
    qDebug()<<"focus button";
    ui->stackedWidget->setCurrentIndex(2);
}

// 相机参数设置
void MainWindow::on_action_camerasetting_triggered()
{
    m_setcameradlg.performDlg();
}

// 新建病人
//void MainWindow::on_pushButton_createperson_clicked()
//{
//    // 设置ID为系统当前时间
//    m_strCurPersonID = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
//    m_strPersonPath = m_strSavePath + m_strCurPersonID;
//    qDebug()<<m_strCurPersonID;
//    m_camerathd.m_vecSnapImgs.clear();

//    // 清空各框
//    ui->lineEdit_name->setText("");
//    ui->comboBox_sex->setCurrentIndex(0);
//    ui->lineEdit_checkpos->setText("");
//    ui->spinBox_age->setValue(40);
//    ui->dateEdit_date->setDate(QDate::currentDate());
//    ui->lineEdit_checkroom->setText("");
//    ui->lineEdit_menzhenno->setText("");
//    ui->lineEdit_zhuyuanno->setText("");
//    ui->lineEdit_bingquno->setText("");
//    ui->lineEdit_chuangweino->setText("");
//    ui->lineEdit_tocheckdoctor->setText("");
//    ui->lineEdit_checkdoctor->setText("");
//    ui->textEdit_comment->setText("");
//    ui->listWidget_historyimage->clear();

//    // 焦点放在姓名上
//    ui->lineEdit_name->setFocus();

//    qDebug()<<"Create a Person.";
//}

// 保存病人信息
//void MainWindow::on_pushButton_saveperson_clicked()
//{
//    // 构造数据
//    QVector<QString> vecdata;
//    vecdata.clear();
//    vecdata.push_back(m_strCurPersonID);
//    vecdata.push_back(ui->lineEdit_name->text());
//    vecdata.push_back(ui->comboBox_sex->currentText());
//    vecdata.push_back(ui->lineEdit_checkpos->text());
//    vecdata.push_back(QString("%1").arg(ui->spinBox_age->value()));
//    vecdata.push_back(ui->dateEdit_date->date().toString("yyyy/MM/dd"));
//    vecdata.push_back(ui->lineEdit_checkroom->text());
//    vecdata.push_back(ui->lineEdit_menzhenno->text());
//    vecdata.push_back(ui->lineEdit_zhuyuanno->text());
//    vecdata.push_back(ui->lineEdit_bingquno->text());
//    vecdata.push_back(ui->lineEdit_chuangweino->text());
//    vecdata.push_back(ui->lineEdit_tocheckdoctor->text());
//    vecdata.push_back(ui->lineEdit_checkdoctor->text());
//    vecdata.push_back(ui->textEdit_comment->toPlainText());
//    vecdata.push_back(m_strPersonPath);

//    // 插入数据库
//    if(DataBase::getInstance()->queryItemByID(m_strCurPersonID))
//        DataBase::getInstance()->updateItem(vecdata); // 更新
//    else
//        DataBase::getInstance()->insertItem(vecdata); // 插入

//    // 保存图像
//    m_camerathd.saveImages(m_strPersonPath);
//}

//// 病例浏览选项卡
//void MainWindow::on_pushButton_illbrower_clicked()
//{
//    m_bIsPersonInfo = false;
//    ui->stackedWidget->setCurrentIndex(2);
//    ui->tabWidget_show->setCurrentIndex(1);

//    // 更新数据库
//    queryPersonlist("");
//}

//// 查询病例
//void MainWindow::on_pushButton_query_clicked()
//{
//    QString strname = ui->lineEdit_inputname->text();
//    queryPersonlist(strname);
//}

//// 病例查询与更新
//void MainWindow::queryPersonlist(QString strname)
//{
//    // 查询数据库
//    m_tableData.clear();
//    DataBase::getInstance()->queryAllItems(m_tableData, strname);
//    qDebug()<<"total count is "<<m_tableData.size();

//    // 显示
//    int icolumn = DataBase::getInstance()->m_iColumns-2; // 减2是因为不显示ID和路径
//    ui->tableWidget_personlist->clear();
//    ui->tableWidget_personlist->setRowCount(m_tableData.size());
//    ui->tableWidget_personlist->setColumnCount(icolumn);
//    QString str = "姓名,性别,部位,年龄,日期,送检科室,门诊编号,住院编号,病区编号,床位编号,送检医生,医生,备注信息";
//    QStringList strlist = str.split(",");
//    ui->tableWidget_personlist->setHorizontalHeaderLabels(strlist);
//    for(int i = 0; i < m_tableData.size(); i++)
//        for(int j = 0; j < icolumn; j++)
//            ui->tableWidget_personlist->setItem(i, j, new QTableWidgetItem(m_tableData[m_tableData.size()-1-i][j+1]));

//    // 加载第一个病人的图片
//    if(m_tableData.size() > 0)
//    {
//        int ind = m_tableData.size() - 1; // 选中最后一个
//        loadUserImgs(m_tableData[m_tableData.size() - 1-ind][m_tableData[0].size()-1]);
//        ui->tableWidget_personlist->setCurrentCell(ind, 0);
//    }
//    else
//    {
//        ui->listWidget_historyimage->clear();
//        OpenCVTool::showImage(m_matInitImg, ui->label_picture);
//    }
//}

//// 加载病案图像
//void MainWindow::loadUserImgs(QString strPath)
//{
//    ui->listWidget_historyimage->clear();
//    m_vecUserImgs.clear();
//    qDebug()<<strPath;

//    // 判断文件夹是否存在
//    QDir dir(strPath);
//    if(dir.exists())
//    {
//        // 遍历文件夹
//        QStringList filters;
//        filters<<QString("*.png");
//        dir.setNameFilters(filters);
//        dir.setFilter(QDir::Files);
//        QString strname, strfile;
//        foreach(QFileInfo mfi, dir.entryInfoList())
//        {
//            // 读取图像
//            strname = mfi.fileName();
//            //            qDebug()<< "File :" << strname;
//            strfile = strPath + "//" + strname;
//            Mat img = imread(strfile.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
//            if(!img.empty())
//            {
//                m_vecUserImgs.push_back(img);

//                // 添加缩略图
//                QImage qimg = OpenCVTool::Mat2QImage(img);
//                updateHistoryList(QIcon(QPixmap::fromImage(qimg).scaled(QSize(100,100))),
//                                  strname.left(strname.size()-4));
//            }
//        }

//        // 图片查看框显示病人的第一个图片
//        if(!m_vecUserImgs.isEmpty())
//            OpenCVTool::showImage(m_vecUserImgs[0], ui->label_picture);
//        else
//            OpenCVTool::showImage(m_matInitImg, ui->label_picture);
//    }
//}

//// 单击病例
//void MainWindow::on_tableWidget_personlist_clicked(const QModelIndex &index)
//{
//    int ind = index.row();
//    if(!m_tableData.empty() && ind < m_tableData.size() && ind >= 0)
//    {
//        ind = m_tableData.size() - 1 - ind;
//        loadUserImgs(m_tableData[ind][m_tableData[ind].size()-1]);
//    }
//}

//// 双击病例
//void MainWindow::on_tableWidget_personlist_doubleClicked(const QModelIndex &index)
//{
//    ui->stackedWidget->setCurrentIndex(0);
//    ui->stackedWidget_buttons->setCurrentIndex(1);

//    int ind = index.row();
//    if(!m_tableData.empty() && ind < m_tableData.size() && ind >= 0)
//    {
//        m_iSelectItem = m_tableData.size() - 1 - ind;
//        ui->lineEdit_name->setText(m_tableData[ind][1]);
//        ui->comboBox_sex->setCurrentText(m_tableData[ind][2]);
//        ui->lineEdit_checkpos->setText(m_tableData[ind][3]);
//        ui->spinBox_age->setValue(m_tableData[ind][4].toInt());
//        ui->dateEdit_date->setDate(QDate().fromString(m_tableData[ind][5]));
//        ui->lineEdit_checkroom->setText(m_tableData[ind][6]);
//        ui->lineEdit_menzhenno->setText(m_tableData[ind][7]);
//        ui->lineEdit_zhuyuanno->setText(m_tableData[ind][8]);
//        ui->lineEdit_bingquno->setText(m_tableData[ind][9]);
//        ui->lineEdit_chuangweino->setText(m_tableData[ind][10]);
//        ui->lineEdit_tocheckdoctor->setText(m_tableData[ind][11]);
//        ui->lineEdit_checkdoctor->setText(m_tableData[ind][12]);
//        ui->textEdit_comment->setText(m_tableData[ind][13]);

//        QString str = QString("%1").arg(m_vecUserImgs.size());
//        ui->label_imgcount->setText(str);
//    }
//}

//// 切换按钮组
//void MainWindow::on_stackedWidget_buttons_currentChanged(int arg1)
//{
//    bool flag = (arg1 == 1);
//    ui->lineEdit_name->setDisabled(flag);
//    ui->comboBox_sex->setDisabled(flag);
//    ui->lineEdit_checkpos->setDisabled(flag);
//    ui->spinBox_age->setDisabled(flag);
//    ui->dateEdit_date->setDisabled(flag);
//    ui->lineEdit_checkroom->setDisabled(flag);
//    ui->lineEdit_menzhenno->setDisabled(flag);
//    ui->lineEdit_zhuyuanno->setDisabled(flag);
//    ui->lineEdit_bingquno->setDisabled(flag);
//    ui->lineEdit_chuangweino->setDisabled(flag);
//    ui->lineEdit_tocheckdoctor->setDisabled(flag);
//    ui->lineEdit_checkdoctor->setDisabled(flag);
//    ui->textEdit_comment->setDisabled(flag);
//}

//// 返回病例浏览
//void MainWindow::on_pushButton_returnpage_clicked()
//{
//    on_pushButton_illbrower_clicked();
//}

//// 删除数据项
//void MainWindow::on_pushButton_deleteitem_clicked()
//{
//    // 删除数据项
//    if(!m_tableData.empty() && m_iSelectItem < m_tableData.size() && m_iSelectItem >= 0)
//        DataBase::getInstance()->deleteItem(m_tableData[m_tableData.size() - 1-m_iSelectItem][0]);

//    // 删除后返回
//    on_pushButton_returnpage_clicked();
//}

//// 导出数据
//void MainWindow::on_pushButton_exportdata_clicked()
//{

//}

// 系统时间设置
void MainWindow::on_action_settime_triggered()
{
    SetTimeDlg timedlg;
    if(timedlg.exec() == QDialog::Accepted)
        qDebug()<<"OK.";
}

// 投影仪设置
void MainWindow::on_action_setprojector_triggered()
{
    m_setprojectordlg.performDlg();
}

// 仪器校准
void MainWindow::on_action_calib_triggered()
{
    m_camerathd.m_iCalibration = -1;
    ui->stackedWidget->setCurrentIndex(3);
}

//// 数据统计
//void MainWindow::on_action_datastat_triggered()
//{
//    DataStatDlg statdlg;
//    statdlg.performDlg();
//}

//// 资料管理
//void MainWindow::on_action_datamanage_triggered()
//{
//    DataManageDlg managedlg;
//    managedlg.performDlg();

//    // 更新数据库
//    if(ui->stackedWidget->currentIndex() == 2)
//        on_pushButton_illbrower_clicked();
//}

// 打开串口
void MainWindow::openSerialPort()
{
    //查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            qDebug()<<serial.portName();
            serial.close();
        }
    }

    // 打开串口
    m_pSerialPort = new QSerialPort;
    m_pSerialPort->setPortName("COM1");                 // 设置串口名
    m_pSerialPort->setBaudRate(QSerialPort::Baud9600);  // 设置波特率:9600
    m_pSerialPort->setDataBits(QSerialPort::Data8);     // 设置数据位数:8
    m_pSerialPort->setParity(QSerialPort::NoParity);    // 设置奇偶校验:无
    m_pSerialPort->setStopBits(QSerialPort::OneStop);   // 设置停止位:1
    m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl); //设置流控制
    connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(readSerialData())); // 连接信号槽
    m_pSerialPort->open(QIODevice::ReadWrite);          // 打开串口
}

// 关闭串口
void MainWindow::closeSerialPort()
{
    if(m_pSerialPort->isOpen())
    {
        m_pSerialPort->clear();
        m_pSerialPort->close();
    }
}

// 接收数据
void MainWindow::readSerialData()
{
    QByteArray buf;
    buf = m_pSerialPort->readAll();
    if(!buf.isEmpty())
    {
        qDebug()<<"read serial - "<<buf;
        if(buf == "$RECER*43\r\n") // 重新发送之前的命令
            sendSerialData(m_strSendCommand);
        else if(buf == "$STEPM,C*30\r\n") // 电机移动完成
        {
            qDebug()<<"motor move over.";
            m_camerathd.m_bMotorFinish = true;
        }
    }
}

// 发送数据
void MainWindow::sendSerialData(QString strsend)
{
    if(m_pSerialPort->isOpen())
        m_pSerialPort->write(strsend.toLatin1());
}

// 向串口发命令
void MainWindow::sendCommand(QString command, QString arg1, QString arg2)
{
    if(!command.isEmpty())
    {
        // 拼字符串
        if(!arg1.isEmpty())
            command = QString("%1,%2").arg(command).arg(arg1);
        if(!arg2.isEmpty())
            command = QString("%1,%2").arg(command).arg(arg2);

        // 异或计算校验和
        QByteArray bytes = command.toLocal8Bit();
        char checksum = bytes[0];
        for(int i = 1; i < bytes.length(); i++)
            checksum = checksum^bytes[i];

        // 形成最后命令
        m_strSendCommand = QString("$%1*%2\r\n").arg(command).arg(QString().sprintf("%02X", checksum));

        // 通过串口发送
        sendSerialData(m_strSendCommand);
        qDebug()<<"send command - "<<m_strSendCommand;
    }
}

//// 更新历史列表
//void MainWindow::updateHistoryList(QIcon icon, QString strname)
//{
//    QString str = QString("%1_%2").arg(ui->listWidget_historyimage->count()+1).arg(strname);
//    QListWidgetItem *item = new QListWidgetItem(icon, str);
//    ui->listWidget_historyimage->addItem(item);
//    ui->listWidget_historyimage->setCurrentRow(ui->listWidget_historyimage->count()-1); // 选中最后一个
//}

//// 抓取图像后更新历史列表
//void MainWindow::updateHistoryList_Capture(QIcon icon, QString strname)
//{
//    // 更新历史列表
//    updateHistoryList(icon, strname);

//    // 保存图像
//    m_camerathd.saveImages(m_strPersonPath); // 随时保存
//}

//// 上一个图片
//void MainWindow::on_pushButton_historypre_clicked()
//{
//    int irow = ui->listWidget_historyimage->currentRow();
//    if(irow > 0)
//        ui->listWidget_historyimage->setCurrentRow(irow-1);
//}

//// 下一个图片
//void MainWindow::on_pushButton_historynext_clicked()
//{
//    int irow = ui->listWidget_historyimage->currentRow();
//    int icount = ui->listWidget_historyimage->count();
//    if(irow < icount-1)
//        ui->listWidget_historyimage->setCurrentRow(irow+1);
//}

//// 选择了不同的图片
//void MainWindow::on_listWidget_historyimage_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
//{
//    // 选中的变颜色
//    if(ui->listWidget_historyimage->currentRow() >= 0)
//    {
//        if(previous != NULL)
//            previous->setBackgroundColor(QColor(255, 255, 255));
//        current->setBackgroundColor(QColor(0, 128, 0));
//    }
//}

//// 单击在图片查看区显示图片
//void MainWindow::on_listWidget_historyimage_currentRowChanged(int currentRow)
//{
//    //ui->tabWidget_show->setCurrentIndex(1); // 切换到图片显示框
//    if(m_bIsPersonInfo)
//    {
//        if(!m_camerathd.m_vecSnapImgs.isEmpty() && currentRow >= 0 && currentRow < m_camerathd.m_vecSnapImgs.size())
//            OpenCVTool::showImage(m_camerathd.m_vecSnapImgs[currentRow].matImg, ui->label_picture);
//    }
//    else
//    {
//        if(!m_vecUserImgs.isEmpty() && currentRow >= 0 && currentRow < m_vecUserImgs.size())
//            OpenCVTool::showImage(m_vecUserImgs[currentRow], ui->label_picture);
//    }
//}

// X+
void MainWindow::on_pushButton_calibXadd_clicked()
{
    //    if(m_camerathd.m_iStartX < m_camerathd.m_nSizeX-1)
    m_camerathd.m_iStartX++;
    updateProjectData();
}
// X-
void MainWindow::on_pushButton_calibXsub_clicked()
{
    //    if(m_camerathd.m_iStartX > 0)
    m_camerathd.m_iStartX--;
    updateProjectData();
}
// Y+
void MainWindow::on_pushButton_calibYadd_clicked()
{
    //    if(m_camerathd.m_iStartY < m_camerathd.m_nSizeY-1)
    m_camerathd.m_iStartY++;
    updateProjectData();
}
// Y-
void MainWindow::on_pushButton_calibYsub_clicked()
{
    //    if(m_camerathd.m_iStartY > 0)
    m_camerathd.m_iStartY--;
    updateProjectData();
}
// 更新投影数据
void MainWindow::updateProjectData()
{
    ui->label_cutwidth->setText(QString("%1").arg(m_camerathd.m_iProjectWidth));
    ui->label_cutheight->setText(QString("%1").arg(m_camerathd.m_iProjectHeight));
    ui->lineEdit_cutstartx->setText(QString("%1").arg(m_camerathd.m_iStartX));
    ui->lineEdit_cutstarty->setText(QString("%1").arg(m_camerathd.m_iStartY));
    ui->label_cutendx->setText(QString("%1").arg(m_camerathd.m_iStartX+m_camerathd.m_iProjectWidth-1));
    ui->label_cutendy->setText(QString("%1").arg(m_camerathd.m_iStartY+m_camerathd.m_iProjectHeight-1));
    ui->lineEdit_scale->setText(QString("%1").arg(m_camerathd.m_dscale));
}

// 修改投影起始点
void MainWindow::on_lineEdit_cutstartx_textChanged(const QString &arg1)
{
    bool flag = false;
    int istartx = arg1.toInt(&flag);
    if(flag)
    {
        m_camerathd.m_iStartX = istartx;
        updateProjectData();
    }
}
void MainWindow::on_lineEdit_cutstarty_textChanged(const QString &arg1)
{
    bool flag = false;
    int istarty = arg1.toInt(&flag);
    if(flag)
    {
        m_camerathd.m_iStartY = istarty;
        updateProjectData();
    }
}

// 检测多屏幕
void MainWindow::detectMultiScreen()
{
    QDesktopWidget *desktop = QApplication::desktop();
    int screen_count = desktop->screenCount();
    m_iPrimaryScreen = desktop->primaryScreen();
    qDebug()<<"screen count:" <<screen_count;
    qDebug()<<"screen primary:" <<m_iPrimaryScreen;

    m_vecScreenRect.clear();
    for(int i=0; i < screen_count ; i++)
    {
        QRect rect = desktop->screenGeometry(m_iPrimaryScreen + i);
        qDebug()<<"Screen "<<i<<": width = "<<rect.width()<<", height = "<<rect.height();
        m_vecScreenRect.push_back(rect);
    }

    // 854*480 or 1280*1024
    if(m_vecScreenRect[0].width() == 854 && m_vecScreenRect[1].height() == 480)
    {
        ui->groupBox->setMaximumHeight(50);
        ui->groupBox_2->setMaximumWidth(300);
        ui->stackedWidget->setMaximumWidth(300);
    }
    else
    {
        // 待调整？
        ui->groupBox->setMaximumHeight(200);
        ui->groupBox_2->setMaximumWidth(500);
        ui->stackedWidget->setMaximumWidth(500);
    }
}

// 自动对准
void MainWindow::on_pushButton_autofocus_clicked()
{
    m_camerathd.m_iAutoFocus = 1;
}

// 电机控制
void MainWindow::controlMotor(bool bdirection)
{
    if(bdirection)
        sendCommand("STEPM", "F");
    else
        sendCommand("STEPM", "R");
}

// 微调+
void MainWindow::on_pushButton_tuneadd_clicked()
{
    sendCommand("STEPM", "F");
}

// 微调-
void MainWindow::on_pushButton_tunesub_clicked()
{
    sendCommand("STEPM", "R");
}

// 激发光开
void MainWindow::on_pushButton_laseropen_clicked()
{
    //    QString strlight;
    //    if(m_setprojectordlg.getLightSource())
    //        strlight = "7";
    //    else
    //        strlight = "8";
    //    sendCommand("LEDPW", "O", strlight);

    if(m_setprojectordlg.getLightSource())
    {
        sendCommand("LEDPW", "O"); // 开780
        sendCommand("LDDPW", "S"); // 关850
    }
    else
    {
        sendCommand("LDDPW", "O"); // 开850
        sendCommand("LEDPW", "S"); // 关780
    }
}

// 激发光关
void MainWindow::on_pushButton_laserclose_clicked()
{
    //    QString strlight;
    //    if(m_setprojectordlg.getLightSource())
    //        strlight = "7";
    //    else
    //        strlight = "8";
    //    sendCommand("LEDPW", "S", strlight);

//    if(m_setprojectordlg.getLightSource())
        sendCommand("LEDPW", "S"); // 780
//    else
        sendCommand("LDDPW", "S"); // 850
}

// 投影仪开
void MainWindow::on_pushButton_projectoropen_clicked()
{
    sendCommand("PRJCT", "O");
}

// 投影仪关
void MainWindow::on_pushButton_projectorclose_clicked()
{
    sendCommand("PRJCT", "S");
}

// 主界面调整增益
void MainWindow::on_horizontalSlider_gain_valueChanged(int value)
{
    m_setcameradlg.adjustGain(value);
}

// 主界面调整投影仪亮度
void MainWindow::on_horizontalSlider_bright_valueChanged(int value)
{
    m_setprojectordlg.adjustBright(value);
}

//// 开始矫正
//void MainWindow::on_pushButton_startcalib_clicked()
//{
//    m_camerathd.m_iCalibration = 1;
//}

//// 矫正结束
//void MainWindow::on_pushButton_calibok_clicked()
//{
//    m_camerathd.m_iCalibration = 2;
//}

//// 病人信息改变
//void MainWindow::on_lineEdit_name_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strName = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_comboBox_sex_currentIndexChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strSex = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_checkpos_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strCheckpos = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_spinBox_age_valueChanged(int arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        qDebug()<<"on_spinBox_age_valueChanged";
//        m_camerathd.m_strAge = QString::number(arg1);
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_dateEdit_date_dateChanged(const QDate &date)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_dateCheck = date;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_checkroom_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strCheckRoom = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_menzhenno_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strMenzhenno = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_zhuyuanno_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strZhuyuanno = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_bingquno_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strBingquno = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_chuangweino_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        m_camerathd.m_strChuangweino = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_tocheckdoctor_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        //        qDebug()<<"on_lineEdit_tocheckdoctor_textChanged";
//        m_camerathd.m_strTocheckdoctor = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_lineEdit_checkdoctor_textChanged(const QString &arg1)
//{
//    if(m_bIsPersonInfo)
//    {
//        //        qDebug()<<"on_lineEdit_checkdoctor_textChanged";
//        m_camerathd.m_strCheckdoctor = arg1;
//        on_pushButton_saveperson_clicked();
//    }
//}
//void MainWindow::on_textEdit_comment_textChanged()
//{
//    if(m_bIsPersonInfo)
//    {
//        //        qDebug()<<"on_textEdit_comment_textChanged";
//        m_camerathd.m_strComment = ui->textEdit_comment->toPlainText();
//        on_pushButton_saveperson_clicked();
//    }
//}

// 关机
void MainWindow::on_action_close_triggered()
{
    // 弹出提示框
    int ires = QMessageBox::information(NULL, "关机提示", "是否确认关机?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(ires == QMessageBox::Yes)
    {
        // 关闭系统
        QProcess p(0);
        p.start("cmd", QStringList()<<"/c"<<"shutdown -s");
        p.waitForStarted();
        p.waitForFinished();

        // 关闭软件
        close();
    }
}

// 关机
bool MainWindow::shutdownWindows()
{
//    HANDLE hToken;
//    TOKEN_PRIVILEGES tkp;

//    // 获取进程标志
//    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
//        return false;

//    // 获取关机特权的LUID
//    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
//    tkp.PrivilegeCount = 1;
//    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

//    // 获取这个进程的关机特权
//    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
//    if (GetLastError() != ERROR_SUCCESS) return false;

//    // 强制关闭计算机
//    if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
//        return false;
    return true;
}

//// 修改了选项页
//void MainWindow::on_stackedWidget_currentChanged(int arg1)
//{
//    if(arg1 != 3)
//        m_camerathd.m_iCalibration = 0;
//}

//// 拍照
//void MainWindow::on_pushButton_capture_clicked()
//{
//    //    if(m_bIsPersonInfo) // 用户界面才能拍照
//    m_camerathd.setSaveImg();
//}

//// 白光模式
void MainWindow::on_pushButton_whitemode_clicked()
{
    m_camerathd.m_bWhiteMode = !m_camerathd.m_bWhiteMode;
}

// 投影缩小
void MainWindow::on_pushButton_zoomin_clicked()
{
    bool flag = false;
    double scalestep = ui->lineEdit_scalestep->text().toFloat(&flag);
    if(flag)
    {
        m_camerathd.m_dscale -= scalestep;
        if(m_camerathd.m_dscale <= 0.1)
            m_camerathd.m_dscale = 0.1;
        updateProjectData();
    }
}

// 投影放大
void MainWindow::on_pushButton_zoomout_clicked()
{
    bool flag = false;
    double scalestep = ui->lineEdit_scalestep->text().toFloat(&flag);
    if(flag)
    {
        m_camerathd.m_dscale += scalestep;
        if(m_camerathd.m_dscale >= 10)
            m_camerathd.m_dscale = 10;
        updateProjectData();
    }
}

// 手动调整缩放比例
void MainWindow::on_lineEdit_scale_textChanged(const QString &arg1)
{
    bool flag = false;
    double dscale = arg1.toFloat(&flag);
    if(flag && dscale >= 0.1 && dscale <= 2)
    {
        m_camerathd.m_dscale = dscale;
        updateProjectData();
    }
}

// 图像左右翻转
void MainWindow::on_pushButton_projectorflip_clicked()
{
    if(m_camerathd.m_bflip)
        m_camerathd.m_bflip = false;
    else
        m_camerathd.m_bflip = true;
}

// 图像上下翻转
void MainWindow::on_pushButton_projectorflipUD_clicked()
{
    if(m_camerathd.m_bflipUD)
        m_camerathd.m_bflipUD = false;
    else
        m_camerathd.m_bflipUD = true;
}

//// 反色
//void MainWindow::on_pushButton_inversemode_clicked()
//{
//    m_camerathd.m_bInverseGray = !m_camerathd.m_bInverseGray;
//}

//// 增强对比度
//void MainWindow::on_pushButton_imgenhance_clicked()
//{
//    m_camerathd.m_benhance = !m_camerathd.m_benhance;
//}

// 自动增益
void MainWindow::on_pushButton_autogain_clicked()
{
    m_bAutoGain = !m_bAutoGain;
    m_setcameradlg.setAutoGain(m_bAutoGain);
}

// 调整了帧率
void MainWindow::on_horizontalSlider_FrameRate_valueChanged(int value)
{
    m_setcameradlg.setFramerate(value);

    QString str;
    str.sprintf("%.0fms", m_camerathd.calcSampleTime(value)*1000);
    ui->label_framerate->setText(str);
}

void MainWindow::on_action_8_triggered()
{
QMessageBox::about(NULL, "about", "Test123456");
}

//抓取三维场景图像
void MainWindow::grab3dscene()
{
    auto img=this->grab(QRect(796,376,800,631));
    ui->label_4->setPixmap(img);
    qDebug()<<"grab finish";

}
