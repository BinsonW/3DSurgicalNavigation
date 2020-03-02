# Image Capture class
import ptvsd

import cv2
from PyQt5 import QtWidgets, QtCore
#from instrumental import list_instruments
import globalvar as gl
class acquireimg(QtCore.QObject):
    signal_finish=QtCore.pyqtSignal()
    def __init__(self):
        super(acquireimg,self).__init__()
        self.cap = cv2.VideoCapture(0)
        if not self.cap.isOpened():
            print("Cannot open camera")
            exit()      
        self.i=0
    def run(self):
        #ptvsd.debug_this_thread()
        gl._semacq(0)
        ret, gl.frame[self.i]=self.cap.read()
        gl._semrel(1)
        self.signal_finish.emit()
        if self.i==9:
            self.i=0
        else:
            self.i+=1
        