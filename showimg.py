# Show img to screen
import cv2
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtGui import QImage,QPixmap
import globalvar as gl

class showimg(QtCore.QObject):
    signal_finish=QtCore.pyqtSignal()
    def __init__(self,win):
        super(showimg,self).__init__()
        self.win=win        
        self.i=0
    def run(self):
        gl._semacq(1)
        rgb_img = cv2.cvtColor(gl.frame[self.i], cv2.COLOR_BGR2RGB)                                          
        QImg = QImage(rgb_img.data, gl.width, gl.height, gl.width*3,QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(QImg)
        self.win.label.setPixmap(pixmap)
        gl._semrel(0)
        self.signal_finish.emit()
        if i==9:
            i=0
        else:
            i+=1
        