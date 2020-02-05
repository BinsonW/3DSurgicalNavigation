# Image process thread
import cv2
from PyQt5 import QtWidgets, QtCore
# Processframe thread
class processimg(QtCore.QObject):
    signal_finish=QtCore.pyqtSignal()
    def __init__(self):
        super(processimg,self).__init__()
    def findmarker(self):        
        self.signal_finish.emit()
    def solvePnP(self):
        pass