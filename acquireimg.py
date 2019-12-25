# Image acquisition thread
import cv2 as cv
from instrumental import list_instruments
class acquireimg(QtCore.QThread):
    def __init__(self):
        super(acquireimg,self).__init__()
        self.cap = cv.VideoCapture(0)
        if not cap.isOpened():
            print("Cannot open camera")
            exit()
    def run(self):
        pass
