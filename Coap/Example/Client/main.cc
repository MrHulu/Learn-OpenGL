#include "widget/MainWindow.h"

#include <QApplication>
#include <iostream>
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try { SimpleClient c; }
    catch (std::exception &e) { std::cout << e.what() << std::endl; }
    MainWindow w;
    w.show();
    return a.exec();
}
