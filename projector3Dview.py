from PyQt5.QtWidgets import QMainWindow, QAction, QApplication, QWidget, QPushButton, qApp, QLabel, QHBoxLayout, QVBoxLayout, QSplitter
from PyQt5.QtGui import QIcon, QPixmap, QPainter, QImage, QMatrix4x4, QQuaternion, QVector3D, QColor, QGuiApplication
from PyQt5.QtCore import QSize, Qt, QRectF
import sys
from PyQt5.Qt3DCore import QEntity, QTransform, QAspectEngine
from PyQt5.Qt3DRender import QCamera, QCameraLens, QRenderAspect
import PyQt5.Qt3DRender as Qt3DRender
from PyQt5.Qt3DInput import QInputAspect
from PyQt5.Qt3DExtras import QForwardRenderer, QPhongMaterial, QCylinderMesh, QSphereMesh, QTorusMesh, Qt3DWindow, QOrbitCameraController

#This is a Qwidget
class proj3D(QWidget):
    def __init__(self):
        super(proj3D, self).__init__()
        #创建3D场景视图窗口view
        self.view = Qt3DWindow()
        #创建放置3D场景视图窗口的容器container，场景view需要先放在一个容器中
        self.container = self.createWindowContainer(self.view)
        #创建水平布局
        vboxlayout = QHBoxLayout()
        #将3D场景容器加入布局当中
        vboxlayout.addWidget(self.container)
        self.setLayout(vboxlayout)
        # 创建根实体（Root Entity）对象scene，即所谓的“画布”
        self.scene = self.createScene()


        # Camera.
        #initialiseCamera(self.view, self.scene)
        # 将根实体（Root Entity）对象scene，即所谓的“画布”设置到3D场景视图viewer中去
        self.view.setRootEntity(self.scene)

        #创建framegraph
        self.createFramegraph_empty(self.scene, self.view)

        #input settings
        self.inputsetting=QInputAspect(self.scene)
        self.view.registerAspect(self.inputsetting)

            # For camera controls.
        self.camController1 = QOrbitCameraController(self.scene)
        self.camController1.setLinearSpeed(50.0)
        self.camController1.setLookSpeed(180.0)
        self.camController1.setCamera(self.view.camera())

        #设置多视口的Framegraph
    #Qt5.10的Qframegraph改为QframegraphNode ?
    def createFramegraph_empty(self, scene,view):

        self.cleanbuffer=Qt3DRender.QClearBuffers()

        self.camsel1=Qt3DRender.QCameraSelector()


        self.camsel2=Qt3DRender.QCameraSelector()

        self.rendersurfacesel=Qt3DRender.QRenderSurfaceSelector()

        self.viewport0=Qt3DRender.QViewport()
        self.viewport1=Qt3DRender.QViewport()
        self.viewport2=Qt3DRender.QViewport()

        self.renderpassfilter=Qt3DRender.QTechniqueFilter()

        # self.filterkey=Qt3DRender.QFilterKey(self.renderpassfilter)
        # self.renderpassfilter.addMatch(self.filterkey)
        self.cleanbuffer.setClearColor(Qt.lightGray)
        self.cleanbuffer.setBuffers(Qt3DRender.QClearBuffers.ColorDepthBuffer)

        self.camera1=QCamera()
        self.camera1.lens().setPerspectiveProjection(45.0, 16.0 / 9.0, 0.1, 1000.0)
        self.camera1.setPosition(QVector3D(0.0, 0.0, 40.0))
        self.camera1.setViewCenter(QVector3D(0.0, 0.0, 0.0))        

        self.camera2=QCamera()
        self.camera2.lens().setPerspectiveProjection(45.0, 16.0 / 9.0, 0.1, 1000.0)
        self.camera2.setPosition(QVector3D(0.0, 0.0, 40.0))
        self.camera2.setViewCenter(QVector3D(0.0, 0.0, 0.0))   

        # For camera controls.
        self.camController1 = QOrbitCameraController(scene)
        self.camController1.setLinearSpeed(50.0)
        self.camController1.setLookSpeed(180.0)
        self.camController1.setCamera(self.camera1)

        self.camController2 = QOrbitCameraController(scene)
        self.camController2.setLinearSpeed(50.0)
        self.camController2.setLookSpeed(180.0)
        self.camController2.setCamera(self.camera2)

        self.camsel1.setCamera(self.camera1)
        self.camsel2.setCamera(self.camera2)

        self.viewport0.setNormalizedRect(QRectF(0.0,0.0,0.3,0.8))
        self.viewport1.setNormalizedRect(QRectF(0.0,0.0,0.5,0.4))        
        self.viewport2.setNormalizedRect(QRectF(0.5,0.0,0.5,0.4))

        ## set relationship
        self.camsel1.setParent(self.cleanbuffer)
        self.camsel2.setParent(self.cleanbuffer)

        self.cleanbuffer.setParent(self.renderpassfilter)
        self.renderpassfilter.setParent(self.rendersurfacesel)

        self.rendersurfacesel.setParent(self.viewport0)
        self.viewport1.setParent(self.viewport0)
        self.viewport2.setParent(self.viewport0) 





        self.view.setActiveFrameGraph(self.viewport0)


        
        #scene.addComponent(self.framegraphcomponent)

    def createScene(self):
    
        self.rootEntity = QEntity()

        # Material.
        self.material = QPhongMaterial(self.rootEntity)
        # light

        # Torus.
        self.torusEntity = QEntity(self.rootEntity)
        self.torusMesh = QTorusMesh()
        self.torusMesh.setRadius(5)
        self.torusMesh.setMinorRadius(1)
        self.torusMesh.setRings(100)
        self.torusMesh.setSlices(20)

        self.torusTransform = QTransform()
        self.torusTransform.setScale3D(QVector3D(1.5, 1.0, 0.5))
        self.torusTransform.setRotation(QQuaternion.fromAxisAndAngle(QVector3D(1.0, 0.0, 0.0), 45.0))

        #将各渲染组件加入到实体list中去。
        self.torusEntity.addComponent(self.torusMesh)
        self.torusEntity.addComponent(self.torusTransform)
        self.torusEntity.addComponent(self.material)

        # Sphere.
        self.sphereEntity = QEntity(self.rootEntity)
        self.sphereMesh = QSphereMesh()
        self.sphereMesh.setRadius(3)
        #将各渲染组件加入到实体list中去。
        self.sphereEntity.addComponent(self.sphereMesh)
        self.sphereEntity.addComponent(self.material)

        #返回根实体
        return self.rootEntity
        


def initialiseCamera(view, scene):
    # Camera.
    camera = view.camera()
    camera.lens().setPerspectiveProjection(45.0, 16.0 / 9.0, 0.1, 1000.0)
    camera.setPosition(QVector3D(0.0, 0.0, 40.0))
    camera.setViewCenter(QVector3D(0.0, 0.0, 0.0))

    # For camera controls.
    camController = QOrbitCameraController(scene)
    camController.setLinearSpeed(50.0)
    camController.setLookSpeed(180.0)
    camController.setCamera(camera)

# Approach 1 - Integrate Qt3DWindow into a QMainWindow
# class Application(QMainWindow):
#     def __init__(self):
#         super().__init__()
#         view3d = View3D()
#         self.setCentralWidget(view3d)
#         self.show()

# if __name__ == '__main__':
#     app = QApplication(sys.argv)
#     ex = Application()
#     sys.exit(app.exec_())


#Approach 2 - A native Qt3DWindow
#if __name__ == '__main__':
    # app = QGuiApplication(sys.argv)
    # view = Qt3DWindow()

    # scene = createScene()
    # initialiseCamera(view, scene)

    # view.setRootEntity(scene)
    # view.show()

    # sys.exit(app.exec_())