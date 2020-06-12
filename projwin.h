#ifndef PROJWIN_H
#define PROJWIN_H

#include <QMainWindow>
#include "widget3d.h"

namespace Ui {
class projwin;
}

class projwin : public QMainWindow
{
    Q_OBJECT

public:
    explicit projwin(QWidget *parent = nullptr);
    ~projwin();


private:
    Ui::projwin *ui;
    widget3D *widget3d;
};

#endif // PROJWIN_H
