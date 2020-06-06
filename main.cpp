#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <windef.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    if(1)
    {
        //w.setWindowFlags(w.windowFlags() &~ Qt::WindowMinMaxButtonsHint); // 不显示最大和最小化
        //w.setWindowFlags(w.windowFlags() &~ Qt::WindowCloseButtonHint); // 禁止关闭
    }
    else
        w.setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint); // 显示标题，不显示最大、最小和关闭

    // 置顶，最大化
//    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint); // 不要置顶，不然弹出其他对话框
    w.setWindowState(Qt::WindowMaximized); // 窗口最大化
//    w.showFullScreen(); // 全屏显示，会无法输入信息，不知道为什么？
    w.show();

//    w.focusPolicy();
    //w.resize(1024, 760);
    //w.move(-12, 0);

    // 限定鼠标移动范围
    //    RECT mainWinRect; // RECT在windef.h中被定义
    //    mainWinRect.left = (LONG)w.geometry().left();
    //    mainWinRect.right = (LONG)w.geometry().right();
    //    mainWinRect.top = (LONG)w.geometry().top();
    //    mainWinRect.bottom = (LONG)w.geometry().bottom();
    //    ClipCursor(&mainWinRect); // 这是Windows API

    return a.exec();
}
