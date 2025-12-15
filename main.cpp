#include "masterview.h"
#include <qApplication.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MasterView w;
    w.show();
    return a.exec();
}
