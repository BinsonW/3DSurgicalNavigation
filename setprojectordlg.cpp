#include <QDebug>
#include <QSettings>

#include "setprojectordlg.h"
#include "ui_setprojectordlg.h"

SetProjectorDlg::SetProjectorDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetProjectorDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    // 加载参数
    QSettings settings("NavigData.ini", QSettings::IniFormat);
    ui->doubleSpinBox_stdvalue->setValue(settings.value("Project/StdValue", 0.0).toFloat());
    ui->doubleSpinBox_gradient->setValue(settings.value("Project/Gradient", 1.0).toFloat());
}

SetProjectorDlg::~SetProjectorDlg()
{
    // 保存参数
    QSettings settings("NavigData.ini", QSettings::IniFormat);
    settings.setValue("Project/StdValue", ui->doubleSpinBox_stdvalue->value());
    settings.setValue("Project/Gradient", ui->doubleSpinBox_gradient->value());

    delete ui;
}

void SetProjectorDlg::on_horizontalSlider_light_valueChanged(int value)
{
    ui->doubleSpinBox_light->setValue(value*0.1);
}

void SetProjectorDlg::on_doubleSpinBox_light_valueChanged(double arg1)
{
    ui->horizontalSlider_light->setValue(arg1*10);

    emit changeProjectBright(arg1);
}

// 执行对话框
void SetProjectorDlg::performDlg()
{
    int nlight = ui->horizontalSlider_light->value();
    int ngradient = ui->doubleSpinBox_gradient->value();
    int nstdv = ui->doubleSpinBox_stdvalue->value();
    if(exec() == QDialog::Accepted)
        qDebug()<<"OK.";
    else
    {
        // 取消，恢复原始值
        qDebug()<<"Cancel.";
        ui->horizontalSlider_light->setValue(nlight);
        ui->doubleSpinBox_gradient->setValue(ngradient);
        ui->doubleSpinBox_stdvalue->setValue(nstdv);
    }
}

// 根据主界面调整亮度
void SetProjectorDlg::adjustBright(int ind)
{
    double dbright = ui->doubleSpinBox_stdvalue->value() + ind * ui->doubleSpinBox_gradient->value();
    if(dbright < 0)
        dbright = 0;
    else if(dbright > 6)
        dbright = 6;

    ui->doubleSpinBox_light->setValue(dbright);
}

// 返回选择的光源: true-780; flase-850
bool SetProjectorDlg::getLightSource()
{
    bool flag;
    if(ui->radioButton_780->isChecked())
        flag = true;
    else
        flag = false;
    return flag;
}


void SetProjectorDlg::on_doubleSpinBox_stdvalue_valueChanged(double arg1)
{

}
