# Image Capture class
import cv2
from PyQt5 import QtWidgets, QtCore
from instrumental import list_instruments

class acquireimg(QtCore.QObject):
    def __init__(self):
        super(acquireimg,self).__init__()
        self.cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("Cannot open camera")
            exit()
        i=0
    def run(self, cv2.Mat):
        while True:
            capframe.acquire()        
            
