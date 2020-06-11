#ifndef WIDGET3D_H
#define WIDGET3D_H

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>
#include <QWidget>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QPropertyAnimation>

#include "qt3dwindow.h"
#include "qorbitcameracontroller.h"
class widget3D: public QWidget
{
    Q_OBJECT
public:
    widget3D();
    QWidget *container;
    Qt3DCore::QEntity *createScene();
    Qt3DExtras::Qt3DWindow *view;
};

#endif // WIDGET3D_H
