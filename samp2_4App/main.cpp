
#include "qwmainwind.h"

#include <QApplication>
#include <QWidget>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWMainWind w;

    w.show();

    return a.exec();
}
