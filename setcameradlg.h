#ifndef SETCAMERADLG_H
#define SETCAMERADLG_H

#include <QDialog>

#include "camerathread.h"

namespace Ui {
class SetCameraDlg;
}

class SetCameraDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetCameraDlg(QWidget *parent = 0);
    ~SetCameraDlg();

public:
    camerathread *m_pcamerathd;
    void setCameraThd(camerathread *pcamerathd);

    void performDlg();
    void adjustGain(int ind);
    void setAutoGain(bool checked);
    void setFramerate(int arg1);

private slots:
    void on_checkBox_autogain_clicked(bool checked);
    void on_checkBox_autowbalance_clicked(bool checked);
    void on_checkBox_autoexpoture_clicked(bool checked);
    void on_spinBox_gradient_valueChanged(int arg1);
    void on_spinBox_stdvalue_valueChanged(int arg1);

    void on_spinBox_framerate_valueChanged(int arg1);

private:
    Ui::SetCameraDlg *ui;
};

#endif // SETCAMERADLG_H
