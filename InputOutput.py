#Read and write camera parameters, coaxial parameters, STL files
#QDir的currentpath 和path之间的区别，好像是两套数据
from PyQt5 import QtCore
camfile=QtCore.QDir()
camfile.setCurrent("Config")
currentpath=camfile.currentPath()
filelist=camfile.entryList(QtCore.QDir.Files)
print(currentpath)
print(filelist)

#modelfile=Qdir()