#-------------------------------------------------
#
# Project created by QtCreator 2017-03-25T09:04:08
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NavigGUI
TEMPLATE = app

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\" # 提升权限，以修改系统时间

SOURCES += main.cpp\
        mainwindow.cpp \
    camerathread.cpp \
    setcameradlg.cpp \
    database.cpp \
    settimedlg.cpp \
    setprojectordlg.cpp \
    datastatdlg.cpp \
    datamanagedlg.cpp \
    opencvtool.cpp

HEADERS  += mainwindow.h \
    uc480.h \
    uc480_deprecated.h \
    camerathread.h \
    setcameradlg.h \
    database.h \
    settimedlg.h \
    setprojectordlg.h \
    datastatdlg.h \
    datamanagedlg.h \
    opencvtool.h

FORMS    += mainwindow.ui \
    setcameradlg.ui \
    settimedlg.ui \
    setprojectordlg.ui \
    datastatdlg.ui \
    datamanagedlg.ui

LIBS += -LE:\companyprogram\NavigGUI(Guoj) -luc480_64 -luser32# 相机驱动;鼠标禁止移出窗外

#DEFINES += QT_NO_DEBUG_OUTPUT # 屏蔽release版本中所有QDebug()提示信息

RESOURCES += \
    resource.qrc

include (opencv430.pri)
INCLUDEPATH+="$$PWD/opencv/include/"
INCLUDEPATH+="$$PWD/opencv/include/opencv2"

DISTFILES += \
    opencv430.pri
