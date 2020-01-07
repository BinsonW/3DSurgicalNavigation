# Image process thread
import cv2
from PyQt5 import QtWidgets, QtCore
# Processframe thread
class processimg(QtCore.QObject):
    def __init__(self):
        super(processimg,self).__init__()
        
    def run(self,img):
        pass