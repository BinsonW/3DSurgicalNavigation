import ptvsd
import sys
from PyQt5 import QtWidgets, QtCore
from PyQt5.QtGui import QImage,QPixmap
from Ui_navig import Ui_MainWindow
from PIL import Image
import cv2
import time
import globalvar as gl
import acquireimg
import processimg
import numpy as np
import logging

# Work status
isCapture=True
isprocess=False

# UI thread
class UsingUi(QtWidgets.QMainWindow, Ui_MainWindow):
    sig_show_finish=QtCore.pyqtSignal()
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)
        self.i=0
        self.start_time=time.perf_counter()
        self.frame_count=0
    def refreshimg(self):
        gl._semacq(2)
        cv2.cvtColor(gl.frame[self.i], cv2.COLOR_BGR2RGB,gl.frame[self.i])
        # key proccessssssss
        #img=np.require(gl.frame[self.i],np.uint8,'C')      

        QImg = QImage(gl.frame[self.i].data, gl.height, gl.width, gl.height * 3,QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(QImg)
        self.label.setPixmap(pixmap)
        gl._semrel(0)
        self.sig_show_finish.emit()
        if self.i==9:
            self.i=0
        else:
            self.i+=1
    def refreshFPS(self):
        self.current_time=time.perf_counter()
        t=(self.current_time-self.start_time)
        if(t>1):
            FPS=self.frame_count/t
            self.frame_count=0
            self.start_time=time.perf_counter()
            self.label_2.setText('FPS:'+str(FPS))
        else:
            self.frame_count+=1

if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    gl._init()
    acq=acquireimg.acquireimg()
    proc=processimg.processimg()

    #thread setup
    acqthread=QtCore.QThread()
    procthread=QtCore.QThread()
    
    acq.moveToThread(acqthread)
    proc.moveToThread(procthread)

    #connect signal and slot
    acq.signal_finish.connect(proc.findmarker)
    proc.signal_finish.connect(win.refreshimg)
    win.sig_show_finish.connect(acq.run)
    win.sig_show_finish.connect(win.refreshFPS)

    win.show()
    acqthread.start()
    procthread.start()
    acq.run()
    sys.exit(app.exec_())