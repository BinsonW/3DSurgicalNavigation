import sys
from PyQt5 import QtWidgets, QtCore
from Ui_navig import Ui_MainWindow
import cv2 as cv

import acquireimg
import processimg

class UsingUi(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)

        # Initialize img
        self.img1=cv.Mat()
        self.img2=cv.Mat()
        self.inturn=1
        # add img here

        # Initialize acquire thread
        self.acq=acquireimg()

        # 
        self.acq.finished.connect(self)


if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    win.show()
    acq.start()
    sys.exit(app.exec_())