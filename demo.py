from PyQt5.QtWidgets import QMainWindow, QAction, QApplication, QWidget, QPushButton, qApp, QLabel, QHBoxLayout, QVBoxLayout, QSplitter
from PyQt5.QtGui import QIcon, QPixmap, QPainter, QImage, QMatrix4x4, QQuaternion, QVector3D, QColor, QGuiApplication
from PyQt5.QtCore import QSize, Qt, QRectF
import sys
from PyQt5.Qt3DCore import QEntity, QTransform, QAspectEngine
from PyQt5.Qt3DRender import QCamera, QCameraLens, QRenderAspect
import PyQt5.Qt3DRender as Qt3DRender
from PyQt5.Qt3DInput import QInputAspect
from PyQt5.Qt3DExtras import QForwardRenderer, QPhongMaterial, QCylinderMesh, QSphereMesh, QTorusMesh, QPlaneMesh, Qt3DWindow, QOrbitCameraController
class proj3D(QWidget):
    def __init__(self):
        super(proj3D, self).__init__()
        #创建3D场景视图窗口view
        self.view = Qt3DWindow()
        self.cleanbuffer=Qt3DRender.QClearBuffers()
        self.camsel=Qt3DRender.QCameraSelector()
        self.camsel.setCamera(self.view.camera())
        self.rendersurfacesel=Qt3DRender.QRenderSurfaceSelector()
        self.viewport=Qt3DRender.QViewport()
        self.renderpassfilter=Qt3DRender.QTechniqueFilter()

        self.filterkey=Qt3DRender.QFilterKey(self.renderpassfilter)
        self.renderpassfilter.addMatch(self.filterkey)
        self.cleanbuffer.setClearColor(Qt.lightGray)
        self.cleanbuffer.setBuffers(Qt3DRender.QClearBuffers.ColorDepthBuffer)

        self.camsel.setParent(self.cleanbuffer)
        self.cleanbuffer.setParent(self.renderpassfilter)

        self.renderpassfilter.setParent(self.rendersurfacesel)
        self.rendersurfacesel.setParent(self.viewport)

        self.view.setActiveFrameGraph(self.viewport)

        self.rootentiry=QEntity()
        self.view.setRootEntity(self.rootentiry)

        camera=self.view.camera()
        camera.lens().setPerspectiveProjection(45.0, 16.0 / 9.0, 0.1, 1000.0)
        camera.setPosition(QVector3D(0.0, 2.0, 0.0))
        camera.setViewCenter(QVector3D(0.0, 0.0, 0.0))

        self.plane=QEntity(self.rootentiry)

        self.material=QPhongMaterial()
        self.plane.addComponent(self.material)

        self.mesh=QPlaneMesh()
        self.mesh.setWidth(0.3)
        self.mesh.setHeight(0.3)
        self.plane.addComponent(self.mesh)



        #创建放置3D场景视图窗口的容器container，场景view需要先放在一个容器中
        self.container = self.createWindowContainer(self.view)
        #创建水平布局
        vboxlayout = QHBoxLayout()
        #将3D场景容器加入布局当中
        vboxlayout.addWidget(self.container)
        self.setLayout(vboxlayout)


 
# #Approach 1 - Integrate Qt3DWindow into a QMainWindow
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
if __name__ == '__main__':
    app = QApplication(sys.argv)
    view=proj3D()
    view.show()

    sys.exit(app.exec_())