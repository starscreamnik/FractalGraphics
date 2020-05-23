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

#include "surfacegraph.h"
#include "spoiler.h"

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

    std::cout << std::endl <<
                 "    MAX: " << max << std::endl <<
                 "    MIN: " << min << std::endl <<
                 "    AVG: " << avg << std::endl <<
                 "************************************************************" << std::endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q3DSurface* graph = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(graph);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;

    //============= Create Main Layouts =================//
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    hLayout->addWidget(container, 1);

    QVBoxLayout *settingsLayout = new QVBoxLayout;
    QGroupBox* settingsGroup = new QGroupBox();
    settingsGroup->setLayout(settingsLayout);

    hLayout->addWidget(settingsGroup);

    /*Spoiler spoiler("Params", 300, settingsGroup);
    spoiler.setContentLayout(*settingsLayout);*/

    //============= Create Settings Widgets ============//

    // Logs for method's time check **********************
    QGroupBox *logGroup = new QGroupBox(QStringLiteral("Calculation Time Log"));
    QListWidget *log = new QListWidget;

    QVBoxLayout *logLayout = new QVBoxLayout;
    logLayout->addWidget(log);
    logGroup->setLayout(logLayout);


    // Options for the memory initialization *************

    QGroupBox *paramsGroup = new QGroupBox(QStringLiteral("Params"));

    QLabel* divLabel = new QLabel("Divisions");
    QSpinBox* divisionsSB = new QSpinBox(paramsGroup);
    divisionsSB->setValue(64);
    divisionsSB->setMinimum(2);
    divisionsSB->setMaximum(8192);

    QLabel* sizeLabel = new QLabel("Size");
    QDoubleSpinBox* sizeSB = new QDoubleSpinBox(paramsGroup);
    sizeSB->setValue(100.0);
    sizeSB->setMinimum(0.);
    sizeSB->setMaximum(1000.);

    QLabel* heightLabel = new QLabel("Height");
    QDoubleSpinBox* heightSB = new QDoubleSpinBox(paramsGroup);
    heightSB->setValue(10.);
    heightSB->setMinimum(0.);
    heightSB->setMaximum(1000.);

    QGridLayout* paramsLayout = new QGridLayout;
    paramsLayout->addWidget(divLabel, 0, 0);
    paramsLayout->addWidget(divisionsSB, 0, 1);

    paramsLayout->addWidget(sizeLabel, 1, 0);
    paramsLayout->addWidget(sizeSB, 1, 1);

    paramsLayout->addWidget(heightLabel, 2, 0);
    paramsLayout->addWidget(heightSB, 2, 1);

    paramsGroup->setLayout(paramsLayout);

    // Calculation method radio buttons ******************
    QGroupBox *methodsGroup = new QGroupBox(QStringLiteral("Method"));

    QRadioButton *sequenceRB = new QRadioButton(widget);
    sequenceRB->setText(QStringLiteral("Sequence"));
    sequenceRB->setChecked(false);

    QRadioButton *parallelRB = new QRadioButton(widget);
    parallelRB->setText(QStringLiteral("QTConcurrent"));
    parallelRB->setChecked(false);

    QVBoxLayout *methodsLayout = new QVBoxLayout;
    methodsLayout->addWidget(sequenceRB);
    methodsLayout->addWidget(parallelRB);
    methodsGroup->setLayout(methodsLayout);

    // Create Render Format group ************************

    QGroupBox *formatsGroup = new QGroupBox(QStringLiteral("Render Format"));

    QLabel* colLabel = new QLabel("Column Range");

    QSlider *axisMinSliderX = new QSlider(Qt::Horizontal, widget);
    axisMinSliderX->setMinimum(0);
    axisMinSliderX->setTickInterval(1);
    axisMinSliderX->setEnabled(true);
    QSlider *axisMaxSliderX = new QSlider(Qt::Horizontal, widget);
    axisMaxSliderX->setMinimum(1);
    axisMaxSliderX->setTickInterval(1);
    axisMaxSliderX->setEnabled(true);

    QLabel* rowLabel = new QLabel("Row Range");

    QSlider *axisMinSliderZ = new QSlider(Qt::Horizontal, widget);
    axisMinSliderZ->setMinimum(0);
    axisMinSliderZ->setTickInterval(1);
    axisMinSliderZ->setEnabled(true);
    QSlider *axisMaxSliderZ = new QSlider(Qt::Horizontal, widget);
    axisMaxSliderZ->setMinimum(1);
    axisMaxSliderZ->setTickInterval(1);
    axisMaxSliderZ->setEnabled(true);

    QGridLayout* formatsLayout = new QGridLayout;
    formatsLayout->addWidget(colLabel, 0, 0);
    formatsLayout->addWidget(axisMinSliderX, 0, 1);
    formatsLayout->addWidget(axisMaxSliderX, 1, 1);

    formatsLayout->addWidget(rowLabel, 2, 0);
    formatsLayout->addWidget(axisMinSliderZ, 2, 1);
    formatsLayout->addWidget(axisMaxSliderZ, 3, 1);
    formatsGroup->setLayout(formatsLayout);


    QPushButton* calcPB = new QPushButton("Calculate", settingsGroup);

    // Add groups to settings group ****************

    settingsLayout->addWidget(logGroup);
    settingsLayout->addWidget(methodsGroup);
    settingsLayout->addWidget(paramsGroup);
    settingsLayout->addWidget(formatsGroup);
    settingsLayout->addWidget(calcPB);

    widget->setWindowTitle(QStringLiteral("Fractal Graphics"));
    widget->show();

    SurfaceGraph *modifier = new SurfaceGraph(graph, 64, 100.f, 10.f, log);

    QObject::connect(sequenceRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleSequenceMethod);
    QObject::connect(parallelRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleQTConcurrentMethod);

    QObject::connect(divisionsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     modifier, &SurfaceGraph::setSurfaceDivisions);

    QObject::connect(sizeSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     modifier, &SurfaceGraph::setSurfaceSize);

    QObject::connect(heightSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     modifier, &SurfaceGraph::setSurfaceHeight);

    QObject::connect(axisMinSliderX, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustXMin);
    QObject::connect(axisMaxSliderX, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustXMax);
    QObject::connect(axisMinSliderZ, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustZMin);
    QObject::connect(axisMaxSliderZ, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustZMax);

    QObject::connect(calcPB, &QPushButton::released,
                     modifier, &SurfaceGraph::calculate);


    modifier->setAxisMinSliderX(axisMinSliderX);
    modifier->setAxisMaxSliderX(axisMaxSliderX);
    modifier->setAxisMinSliderZ(axisMinSliderZ);
    modifier->setAxisMaxSliderZ(axisMaxSliderZ);

    sequenceRB->setChecked(true);

    return a.exec();
}
