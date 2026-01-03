<<<<<<< HEAD
#include "masterview.h"
#include <qApplication.h>

=======
#include "mainwindow.h"
#include "LoginWindow.h" // 新增：登录窗口头文件
#include <QApplication>
>>>>>>> 56b7f05 (init: 初始化项目结构，配置 Qt6 环境)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
<<<<<<< HEAD
    MasterView w;
    w.show();
=======

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

>>>>>>> 56b7f05 (init: 初始化项目结构，配置 Qt6 环境)
    return a.exec();
}
