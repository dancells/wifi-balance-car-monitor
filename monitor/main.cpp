#include "widget.h"
#include <QApplication>
#include "rocker.h"


int main(int argc, char *argv[])
{
    // 创建应用程序对象
    QApplication a(argc, argv);
    // 创建窗口对象
    Widget w;
    // 显示窗口
    w.show();

    // 进入消息循环
    return a.exec();
}
