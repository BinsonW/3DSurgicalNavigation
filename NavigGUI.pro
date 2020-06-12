QT       += core gui serialport
QT += 3dcore 3drender 3dinput 3dlogic 3dextras 3danimation
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =app

CONFIG += c++11

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\" # 提升权限，以修改系统时间

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
    Frame.cpp \
    Model3D.cpp \
        mainwindow.cpp \
    camerathread.cpp \
    marker.cpp \
    projwin.cpp \
    setcameradlg.cpp \
    settimedlg.cpp \
    setprojectordlg.cpp \
    opencvtool.cpp \
    widget3d.cpp

HEADERS  += mainwindow.h \
    Frame.h \
    Model3D.h \
    projwin.h \
    uc480.h \
    uc480_deprecated.h \
    camerathread.h \
    setcameradlg.h \
    settimedlg.h \
    setprojectordlg.h \
    opencvtool.h \
    widget3d.h

FORMS    += mainwindow.ui \
    projwin.ui \
    setcameradlg.ui \
    settimedlg.ui \
    setprojectordlg.ui

LIBS += -luc480_64 -luser32 # 相机驱动;鼠标禁止移出窗外

#DEFINES += QT_NO_DEBUG_OUTPUT # 屏蔽release版本中所有QDebug()提示信息

RESOURCES += \
    resource.qrc

INCLUDEPATH+="$$PWD/opencv/include/"
INCLUDEPATH+="$$PWD/opencv/include/opencv2"

win32: LIBS += -L$$PWD/opencv/x64/vc16/lib/ -lopencv_world430d

INCLUDEPATH += $$PWD/opencv/x64/vc16
DEPENDPATH += $$PWD/opencv/x64/vc16


win32: LIBS += -L$$PWD/./ -luc480_64

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./uc480_64.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./libuc480_64.a
