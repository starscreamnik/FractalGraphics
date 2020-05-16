#include "mainwindow.h"
#include <QApplication>
#include "diamondsquare.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto obj = DiamondSquare(3, 16, 10);

    return a.exec();
}
