#ifndef SETTIMEDLG_H
#define SETTIMEDLG_H

#include <QDialog>

namespace Ui {
class SetTimeDlg;
}

class SetTimeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetTimeDlg(QWidget *parent = 0);
    ~SetTimeDlg();

private slots:

    void on_pushButton_ok_clicked();

private:
    Ui::SetTimeDlg *ui;
};

#endif // SETTIMEDLG_H
