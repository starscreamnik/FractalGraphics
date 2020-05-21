#include "mainwindow.h"
#include "diamondsquare.h"
#include <QApplication>
#include <QTime>

using namespace std;

template< typename Func, typename... Args >
void benchmark(int iterCount, DiamondSquare& object, Func func, Args... args ) {
    if( iterCount <= 0 ) {
        return;
    }
    QVector< int > elapsedTimes;
    for( int i = 0; i < iterCount; ++i ) {
        QTime time;
        time.start();
        (object.*func)(args...);
        elapsedTimes << time.elapsed();
    }

    double max = *std::max_element( elapsedTimes.constBegin(), elapsedTimes.constEnd() ) / 1000.0;
    double min = *std::min_element( elapsedTimes.constBegin(), elapsedTimes.constEnd() ) / 1000.0;
    int sum = std::accumulate( elapsedTimes.constBegin(), elapsedTimes.constEnd(), 0 );
    double avg = sum / ( iterCount * 1000.0 );

    std::cout << std::endl <<
                 "    MAX: " << max << std::endl <<
                 "    MIN: " << min << std::endl <<
                 "    AVG: " << avg << std::endl <<
                 "************************************************************" << std::endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    auto obj = DiamondSquare(2048, 1000, 100);

    void (DiamondSquare::* func1)() = &DiamondSquare::CreateHeightMap;
    void (DiamondSquare::* func2)() = &DiamondSquare::CreateHeightMapByThreads;

    benchmark(50, obj, func2);

//    obj.WriteHeightMapToFile("C:/Users/Nikita/Desktop/heightmapQT.txt");


    return a.exec();
}
