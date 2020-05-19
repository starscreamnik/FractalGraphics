#include "mainwindow.h"
#include "diamondsquare.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto obj = DiamondSquare(100, 50, 5);
    obj.CreateHeightMap();
    obj.WriteHeightMapToFile("C:/Users/Nikita/Desktop/heightmapQT.txt");

    return a.exec();
}
