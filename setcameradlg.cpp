#include <QDebug>
#include <QSettings>

#include "setcameradlg.h"
#include "ui_setcameradlg.h"

SetCameraDlg::SetCameraDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetCameraDlg)
{
    ui->setupUi(this);
    m_pcamerathd = NULL;
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    // 加载参数
    QSettings settings("NavigData.ini", QSettings::IniFormat);
    ui->spinBox_stdvalue->setValue(settings.value("CameraPara/StdValue", 60).toInt());
    ui->spinBox_gradient->setValue(settings.value("CameraPara/Gradient", 20).toInt());
}

SetCameraDlg::~SetCameraDlg()
{
    // 保存参数
    QSettings settings("NavigData.ini", QSettings::IniFormat);
    settings.setValue("CameraPara/StdValue", ui->spinBox_stdvalue->value());
    settings.setValue("CameraPara/Gradient", ui->spinBox_gradient->value());

    delete ui;
}

// 设置相机线程句柄
void SetCameraDlg::setCameraThd(camerathread *pcamerathd)
{
    m_pcamerathd = pcamerathd;
    if(m_pcamerathd)
    {                
        ui->horizontalSlider_gain->setValue(m_pcamerathd->m_nGain);
        ui->horizontalSlider_whitebalance->setValue(m_pcamerathd->m_nwbalance);
        ui->horizontalSlider_framerate->setValue(m_pcamerathd->m_nframerate);
        ui->horizontalSlider_expoture->setValue(m_pcamerathd->m_nExposure);

        connect(ui->horizontalSlider_expoture, SIGNAL(valueChanged(int)), m_pcamerathd, SLOT(setExposure(int)));
        connect(ui->horizontalSlider_gain, SIGNAL(valueChanged(int)), m_pcamerathd, SLOT(setGain(int)));
        connect(ui->horizontalSlider_whitebalance, SIGNAL(valueChanged(int)), m_pcamerathd, SLOT(setWhiteBalance(int)));
        connect(ui->horizontalSlider_framerate, SIGNAL(valueChanged(int)), m_pcamerathd, SLOT(setFrameRate(int)));
    }
}

// 自动增益
void SetCameraDlg::on_checkBox_autogain_clicked(bool checked)
{
    m_pcamerathd->setAutoGain(checked);
    ui->horizontalSlider_gain->setEnabled(!checked);
    ui->spinBox_gain->setEnabled(!checked);

    if(!checked)
        ui->spinBox_gain->setValue(m_pcamerathd->m_nGain);
}
// 自动白平衡
void SetCameraDlg::on_checkBox_autowbalance_clicked(bool checked)
{
    m_pcamerathd->setAutoWbalance(checked);
    ui->horizontalSlider_whitebalance->setEnabled(!checked);
    ui->spinBox_wbalance->setEnabled(!checked);

    if(!checked)
        ui->spinBox_wbalance->setValue(m_pcamerathd->m_nwbalance);
}
// 自动曝光
void SetCameraDlg::on_checkBox_autoexpoture_clicked(bool checked)
{
    m_pcamerathd->setAutoExpoture(checked);
    ui->horizontalSlider_expoture->setEnabled(!checked);
    ui->spinBox_exposure->setEnabled(!checked);

    if(!checked)
        ui->spinBox_exposure->setValue(m_pcamerathd->m_nExposure);
}

// 基准值
void SetCameraDlg::on_spinBox_stdvalue_valueChanged(int arg1)
{

}

// 调整梯度
void SetCameraDlg::on_spinBox_gradient_valueChanged(int arg1)
{

}

// 执行对话框
void SetCameraDlg::performDlg()
{
    if(m_pcamerathd)
    {
        UINT nExposure = ui->horizontalSlider_expoture->value();
        UINT nGain = ui->horizontalSlider_gain->value();
        UINT nwbalance = ui->horizontalSlider_whitebalance->value();
        UINT nframerate = ui->horizontalSlider_framerate->value();

        qDebug()<<"曝光时间:"<<nExposure;
        if(exec() == QDialog::Accepted)
            qDebug()<<"OK.";
        else
        {
            // 取消，恢复原始值
            qDebug()<<"Cancel.";
            ui->horizontalSlider_expoture->setValue(nExposure);
            ui->horizontalSlider_gain->setValue(nGain);
            ui->horizontalSlider_whitebalance->setValue(nwbalance);
            ui->horizontalSlider_framerate->setValue(nframerate);
        }
    }
}

// 根据主界面调整增益
void SetCameraDlg::adjustGain(int ind)
{
    int igain = ui->spinBox_stdvalue->value() + ind * ui->spinBox_gradient->value();
    if(igain < 0)
        igain = 0;
    else if(igain > 100)
        igain = 100;

    ui->spinBox_gain->setValue(igain);
}

// 设置自动增益
void SetCameraDlg::setAutoGain(bool checked)
{
    ui->checkBox_autogain->setChecked(checked);
}

// 通过主界面调整了帧率
void SetCameraDlg::setFramerate(int arg1)
{
    ui->spinBox_framerate->setValue(arg1);
}

// 调整帧率
void SetCameraDlg::on_spinBox_framerate_valueChanged(int arg1)
{
    ui->horizontalSlider_expoture->setValue(100);

    QString str;
    str.sprintf("%.0fms", m_pcamerathd->calcSampleTime(arg1)*1000);
    ui->label_framerate->setText(str);
}

