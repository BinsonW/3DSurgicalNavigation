import sys
from PyQt5 import QtWidgets, QtCore
from Ui_navig import Ui_MainWindow
import cv2 as cv

import acquireimg
import processimg
# Semaphore
frameBufferSize=10
QtCore.QSemaphore capframe(frameBufferSize)
QtCore.QSemaphore procframe(0)
QtCore.QSemaphore geneframe(0)
QtCore.QSemaphore showframe(0)

# Framebuffer
Frame

# Work status
isCapture=True
isprocess=False

# Captureframe thread
class acquireimg(QtCore.QThread):
    def __init__(self):
        super(acquireimg,self).__init__()

        # Instantialize work class here?
        self.cap=acquireimg()

        # Can this work?
        acquireimg.moveToThread(self)

        i=0
    def run(self):
        while True:
            capframe.acquire()
            cap.run(Frame(i))
            if is2Dprocess==True:
                procframe.release()
            # If manually select marker, add judge here
            else showframe.release()
            if i<10:
                i++
            else i=0





# Processframe thread
class processimg(QtCore.QThread):
    def __init__(self):
        super(processimg,self).__init__()
        
    def run(self,img):
        pass
# UI thread
class UsingUi(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)





        # Initialize acquire thread
        self.acq=acquireimg()

        # Initialize process thread
        self.proc=processimg()
        
        # 
        self.acq.finished.connect(self.)


if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    win.show()
    acq.start()
    sys.exit(app.exec_())