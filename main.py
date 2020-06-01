import ptvsd
import sys
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtGui import QImage,QPixmap
from UI.Ui_navig import Ui_MainWindow
import cv2
import time
import globalvar as gl
import acquireimg
import processimg
import projector3Dview, screen3Dview
import numpy as np
import logging

# Work status
isCapture=True
isprocess=False

# UI thread
class UsingUi(QtWidgets.QMainWindow, Ui_MainWindow):
    sig_showinitimg_finish=QtCore.pyqtSignal()
    sig_showmarkerimg_finish=QtCore.pyqtSignal()
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)
        
        #set table content
        self.markertable=QtGui.QStandardItemModel(16,3)
        self.markertable.setHorizontalHeaderLabels(['X','Y','Z'])
        self.markertable.setVerticalHeaderLabels(['R1',' ','2',' ','3',' ','4',' ','M1',' ','2',' ','3',' ','4',' '])
        #show table
        self.tableView.setModel(self.markertable)
        self.tableView.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.tableView.verticalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.showMaximized()

        self.i0=0
        self.i1=0
        self.start_time=time.perf_counter()
        self.frame_count=0
    def refresh_initimg(self):
        gl._semacq(1)

        cv2.cvtColor(gl.frame[self.i0], cv2.COLOR_BGR2RGB,gl.frame[self.i0])
        # key proccessssssss
        #img=np.require(gl.frame[self.i],np.uint8,'C')      

        QImg = QtGui.QImage(gl.frame[self.i0].data, gl.height, gl.width, gl.height * 3,QImage.Format_RGB888)
        pixmap = QtGui.QPixmap.fromImage(QImg)
        self.label.setPixmap(pixmap)
        gl._semrel(2)
        self.sig_showinitimg_finish.emit()
        if self.i0==9:
            self.i0=0
        else:
            self.i0+=1
    def refresh_markerimg(self):
        gl._semacq(3)
        cv2.cvtColor(gl.frame_proc[self.i1], cv2.COLOR_BGR2RGB,gl.frame_proc[self.i1])
        # key proccessssssss
        #img=np.require(gl.frame[self.i],np.uint8,'C')      

        QImg = QImage(gl.frame_proc[self.i1].data, gl.height, gl.width, gl.height * 3,QImage.Format_RGB888)
        pixmap = QPixmap.fromImage(QImg)
        self.label_2.setPixmap(pixmap)
        gl._semrel(0)
        self.sig_showmarkerimg_finish.emit()
        if self.i1==9:
            self.i1=0
        else:
            self.i1+=1
    def refreshFPS(self):
        self.current_time=time.perf_counter()
        t=(self.current_time-self.start_time)
        if(t>1):
            FPS=self.frame_count/t
            self.frame_count=0
            self.start_time=time.perf_counter()
            self.frame.setText('FPS:'+str(FPS))
        else:
            self.frame_count+=1

if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    gl._init()
    acq=acquireimg.acquireimg()
    proc=processimg.processimg()
    proj3D=projector3Dview.proj3D()
    #screen3d=screen3Dview.screen3D()
    #add widget
    win.gridLayout.addWidget(proj3D,1,0,-1,-1)
    #win.gridLayout.addWidget(screen3d,1,1)
    #thread setup
    acqthread=QtCore.QThread()
    procthread=QtCore.QThread()
    
    acq.moveToThread(acqthread)
    proc.moveToThread(procthread)

    #connect signal and slot
    acq.signal_finish.connect(win.refresh_initimg)
    win.sig_showinitimg_finish.connect(proc.findmarker)    
    proc.signal_finish.connect(win.refresh_markerimg)
    #proc.signal_finish.connect(solvePNP)
    win.sig_showmarkerimg_finish.connect(acq.run)
    #solvepnp.signal_finish.connect(3drefresh)
    win.sig_showmarkerimg_finish.connect(win.refreshFPS)

    win.show()
    acqthread.start()
    procthread.start()
    
    acq.run()
    sys.exit(app.exec_())