#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QSortPolicy>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QTechniqueFilter>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QTexture>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QTechnique>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    auto view = new Qt3DExtras::Qt3DWindow();
    auto mClearBuffers = new Qt3DRender::QClearBuffers;
    auto mMainCameraSelector = new Qt3DRender::QCameraSelector;
    mMainCameraSelector->setCamera(view->camera());
    auto mRenderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    auto mMainViewport = new Qt3DRender::QViewport;
    auto renderPassFilter = new Qt3DRender::QTechniqueFilter;
    {
        auto filterKey = new Qt3DRender::QFilterKey(renderPassFilter);
        filterKey->setName(QStringLiteral("renderingStyle"));
        filterKey->setValue(QStringLiteral("forward"));
        // Adding the filterKey to the renderPassFilter hides the plane
        // Name and Value of filterKey matches the FilterKey inside the QDiffuseSpecularMaterial
        renderPassFilter->addMatch(filterKey); // Removing this lines shows the plane mesh

        mClearBuffers->setClearColor(Qt::lightGray);
        mClearBuffers->setBuffers(Qt3DRender::QClearBuffers::BufferType::ColorDepthBuffer);
        mMainCameraSelector->setParent(mClearBuffers);
        mClearBuffers->setParent(renderPassFilter);
    }
    renderPassFilter->setParent(mRenderSurfaceSelector);

    mRenderSurfaceSelector->setParent(mMainViewport);

    view->setActiveFrameGraph(mMainViewport);
    view->activeFrameGraph()->dumpObjectTree();

    auto rootEntity = new Qt3DCore::QEntity();
    view->setRootEntity(rootEntity);

    view->camera()->lens()->setPerspectiveProjection(45.0f, 1., 0.1f, 10000.0f);
    view->camera()->setPosition(QVector3D(0, 2, 0));
    view->camera()->setUpVector(QVector3D(0, 1, 0));
    view->camera()->setViewCenter(QVector3D(0, 0, 0));

    auto planeEntity = new Qt3DCore::QEntity(rootEntity);

    auto meshMaterial = new Qt3DExtras::QDiffuseSpecularMaterial;
    meshMaterial->setDiffuse(QColor("#ff00ff"));
    planeEntity->addComponent(meshMaterial);

    auto mesh = new Qt3DExtras::QPlaneMesh;
    mesh->setWidth(0.3);
    mesh->setHeight(0.3);
    planeEntity->addComponent(mesh);

    auto container = QWidget::createWindowContainer(view);
    QFrame frame;
    frame.setLayout(new QVBoxLayout);
    frame.layout()->addWidget(container);
    frame.resize(QSize(400, 300));

    frame.show();
    return a.exec();