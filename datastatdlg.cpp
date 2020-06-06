#include <QDebug>

#include "datastatdlg.h"
#include "ui_datastatdlg.h"

#include "database.h"

DataStatDlg::DataStatDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataStatDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

DataStatDlg::~DataStatDlg()
{
    delete ui;
}

// 执行对话框
void DataStatDlg::performDlg()
{
    int icount = 0;
    qint64 isize = 0;
    DataBase::getInstance()->queryMemory(icount, isize);
    ui->label_count->setText(QString("%1").arg(icount));
    ui->label_memory->setText(QString("%1K").arg(isize));
    exec();
}

