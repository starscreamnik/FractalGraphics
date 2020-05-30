#include <QApplication>
#include <QTime>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtDataVisualization/Q3DSurface>
#include <QDoubleSpinBox>
#include <QListWidget>
#include <iostream>

#include "surfaceView.h"

using namespace QtDataVisualization;
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

    cout << endl <<
                 "    MAX: " << max << endl <<
                 "    MIN: " << min << endl <<
                 "    AVG: " << avg << endl <<
                 "************************************************************" << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SurfaceView* mainView = new SurfaceView();

    return a.exec();
}
