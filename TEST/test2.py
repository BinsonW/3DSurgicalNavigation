from PyQt5 import QtWidgets, QtCore
from Ui_untitled import Ui_MainWindow
import sys
class UsingUi(QtWidgets.QMainWindow,Ui_MainWindow):
    def __init__(self, parent=None):
        super(UsingUi, self).__init__(parent)
        self.setupUi(self)
if __name__ == '__main__':  # entrance
    app = QtWidgets.QApplication(sys.argv)
    win = UsingUi()
    win.show()
    sys.exit(app.exec_())