#include <QDebug>
#include <QDate>

#include "datamanagedlg.h"
#include "ui_datamanagedlg.h"
#include "database.h"

DataManageDlg::DataManageDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataManageDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

DataManageDlg::~DataManageDlg()
{
    delete ui;
}

// 执行对话框
void DataManageDlg::performDlg()
{
    if(exec() == QDialog::Accepted)
    {
        QString strdate;
        if(ui->radioButton_day->isChecked()) // 删几天前的
            strdate = QDate::currentDate().addDays(-ui->spinBox_day->value()).toString("yyyy/MM/dd");
        else if(ui->radioButton_month->isChecked()) // 删几个月前的
            strdate = QDate::currentDate().addMonths(-ui->spinBox_month->value()).toString("yyyy/MM/dd");
        else if(ui->radioButton_year->isChecked()) // 删几年前的
            strdate = QDate::currentDate().addYears(-ui->spinBox_year->value()).toString("yyyy/MM/dd");
        else
            strdate = ""; // 全删
        DataBase::getInstance()->deleteItems(strdate);        
    }
}
