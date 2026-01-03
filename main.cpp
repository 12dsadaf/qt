

#include <qApplication.h>

#include "mainwindow.h"
#include "LoginWindow.h" // 新增：登录窗口头文件
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    // 1. 先显示登录窗口
    LoginWindow loginWnd;
    if (loginWnd.exec()!= QDialog::Accepted) {
        // 登录取消或失败，退出程序
        return 0;
    }

    // 2. 登录成功，显示主窗口
    MainWindow w;
    w.setWindowTitle("个人工作与任务管理系统");
    w.resize(800, 600);
    w.show();


    return a.exec();
}
