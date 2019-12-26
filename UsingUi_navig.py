import sys
from PyQt5 import QtWidgets, QtCore
from Ui_navig import Ui_MainWindow
import cv2 as cv

import acquireimg

class UsingUi(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)

        # Initialize img
        self.img=cv.Mat()

        # Initialize acquire thread
        self.acq=acquireimg(self.img)

        # 
        self.acq.finished.connect(self.imgshow)


if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    win.show()
    sys.exit(app.exec_())