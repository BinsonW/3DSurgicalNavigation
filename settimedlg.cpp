#include <QDateTime>
#include <Windows.h>
#include <QDebug>

#include "settimedlg.h"
#include "ui_settimedlg.h"

SetTimeDlg::SetTimeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetTimeDlg)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    ui->dateTimeEdit_datetime->setDateTime(QDateTime::currentDateTime());
}

SetTimeDlg::~SetTimeDlg()
{
    delete ui;
}

void SetTimeDlg::on_pushButton_ok_clicked()
{
    QDateTime datetime = ui->dateTimeEdit_datetime->dateTime();
    SYSTEMTIME   st;
    GetSystemTime(&st);
    st.wYear = datetime.date().year();
    st.wMonth = datetime.date().month();
    st.wDay = datetime.date().day();
    st.wHour = datetime.time().hour();
    st.wMinute = datetime.time().minute();
    st.wSecond = datetime.time().second();
    SetLocalTime(&st);
}
