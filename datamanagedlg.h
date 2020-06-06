#ifndef DATAMANAGEDLG_H
#define DATAMANAGEDLG_H

#include <QDialog>

namespace Ui {
class DataManageDlg;
}

class DataManageDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DataManageDlg(QWidget *parent = 0);
    ~DataManageDlg();

    void performDlg();
private:
    Ui::DataManageDlg *ui;
};

#endif // DATAMANAGEDLG_H
