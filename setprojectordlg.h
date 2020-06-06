#ifndef SETPROJECTORDLG_H
#define SETPROJECTORDLG_H

#include <QDialog>

namespace Ui {
class SetProjectorDlg;
}

class SetProjectorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetProjectorDlg(QWidget *parent = 0);
    ~SetProjectorDlg();

    void performDlg();
    void adjustBright(int ind);
    bool getLightSource();
private slots:
    void on_horizontalSlider_light_valueChanged(int value);

    void on_doubleSpinBox_light_valueChanged(double arg1);

    void on_doubleSpinBox_stdvalue_valueChanged(double arg1);

signals:
    void changeProjectBright(double);

private:
    Ui::SetProjectorDlg *ui;
};

#endif // SETPROJECTORDLG_H
