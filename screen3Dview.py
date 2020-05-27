from PyQt5.QtWidgets import QMainWindow, QAction, QApplication, QWidget, QPushButton, qApp, QLabel, QHBoxLayout, QVBoxLayout, QSplitter
from PyQt5.QtGui import QIcon, QPixmap, QPainter, QImage, QMatrix4x4, QQuaternion, QVector3D, QColor, QGuiApplication
from PyQt5.QtCore import QSize, Qt
import sys
from PyQt5.Qt3DCore import QEntity, QTransform, QAspectEngine
from PyQt5.Qt3DRender import QCamera, QCameraLens, QRenderAspect
from PyQt5.Qt3DInput import QInputAspect
from PyQt5.Qt3DExtras import QForwardRenderer, QPhongMaterial, QCylinderMesh, QSphereMesh, QTorusMesh, Qt3DWindow, QOrbitCameraController

#This is a Qwidget
class screen3D(QWidget):
    def __init__(self):
        super(screen3D, self).__init__()
        #创建3D场景视图窗口view
        self.view = Qt3DWindow()
        #创建放置3D场景视图窗口的容器container，场景view需要先放在一个容器中
        self.container = self.createWindowContainer(self.view)
        #创建水平布局
        vboxlayout = QHBoxLayout()
        #将3D场景容器加入布局当中
        vboxlayout.addWidget(self.container)
        self.setLayout(vboxlayout)
        # 创建根实体（Root Entity）对象，即所谓的“画布”
        self.scene = createScene()

        # Camera.
        initialiseCamera(self.view, self.scene)
        # 将根实体（Root Entity）对象scene，即所谓的“画布”设置到3D场景视图viewer中去
        self.view.setRootEntity(self.scene)

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

def createScene():
    
    rootEntity = QEntity()

    # Material.
    material = QPhongMaterial(rootEntity)
    # light

    # Torus.
    torusEntity = QEntity(rootEntity)
    torusMesh = QTorusMesh()
    torusMesh.setRadius(5)
    torusMesh.setMinorRadius(1)
    torusMesh.setRings(100)
    torusMesh.setSlices(20)

    torusTransform = QTransform()
    torusTransform.setScale3D(QVector3D(1.5, 1.0, 0.5))
    torusTransform.setRotation(
            QQuaternion.fromAxisAndAngle(QVector3D(1.0, 0.0, 0.0), 45.0))

    #将各渲染组件加入到实体list中去。
    torusEntity.addComponent(torusMesh)
    torusEntity.addComponent(torusTransform)
    torusEntity.addComponent(material)

    # Sphere.
    sphereEntity = QEntity(rootEntity)
    sphereMesh = QSphereMesh()
    sphereMesh.setRadius(3)
    #将各渲染组件加入到实体list中去。
    sphereEntity.addComponent(sphereMesh)
    sphereEntity.addComponent(material)
    #返回根实体
    return rootEntity

class Application(QMainWindow):
    def __init__(self):
        super().__init__()
        view3d = View3D()
        self.setCentralWidget(view3d)
        self.show()

# Approach 1 - Integrate Qt3DWindow into a QMainWindow
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = Application()
    sys.exit(app.exec_())


#Approach 2 - A native Qt3DWindow
#if __name__ == '__main__':
    # app = QGuiApplication(sys.argv)
    # view = Qt3DWindow()

    # scene = createScene()
    # initialiseCamera(view, scene)

    # view.setRootEntity(scene)
    # view.show()

    # sys.exit(app.exec_())