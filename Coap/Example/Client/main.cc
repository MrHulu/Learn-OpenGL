#include "widget/MainWindow.h"

#include <QApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SimpleClient c;
    MainWindow w;
    w.show();
    return a.exec();
}
