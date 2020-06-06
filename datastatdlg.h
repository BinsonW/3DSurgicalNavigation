#ifndef DATASTATDLG_H
#define DATASTATDLG_H

#include <QDialog>

namespace Ui {
class DataStatDlg;
}

class DataStatDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DataStatDlg(QWidget *parent = 0);
    ~DataStatDlg();

    void performDlg();
private:
    Ui::DataStatDlg *ui;
};

#endif // DATASTATDLG_H
