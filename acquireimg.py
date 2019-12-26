# Image acquisition thread
import cv2
from PyQt5 import QtWidgets, QtCore
from instrumental import list_instruments
class acquireimg(QtCore.QThread):
    def __init__(self,img):
        super(acquireimg,self).__init__()
        self.cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("Cannot open camera")
            exit()
    def run(self):
        ret,img=cap.read()
        while:
            
