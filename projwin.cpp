#include "projwin.h"
#include "ui_projwin.h"

projwin::projwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::projwin)
{
    ui->setupUi(this);
    move(-500,0);
    showFullScreen();
    widget3d=new widget3D();
    ui->horizontalLayout->removeWidget(ui->widget);
    ui->horizontalLayout->addWidget(widget3d->container);
}

projwin::~projwin()
{
    delete ui;
}
