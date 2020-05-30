#ifndef SURFACEVIEW_H
#define SURFACEVIEW_H
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

#include "surfaceController.h"

using namespace QtDataVisualization;


class SurfaceView{
public:
    SurfaceView();
    ~SurfaceView();

    Q3DSurface* GetGraph();
    QListWidget* GetLogs();

private:
    Q3DSurface* graph;
    QWidget* container;
    QWidget *widget;
    QVBoxLayout *settingsLayout;
    SurfaceController* modifier;
    QListWidget* logs;
    QRadioButton *sequenceRB;
    QRadioButton *parallelRB;
    QSpinBox* divisionsSB;
    QDoubleSpinBox* sizeSB;
    QDoubleSpinBox* heightSB;
    QSlider *axisMinSliderX;
    QSlider *axisMaxSliderX;
    QSlider *axisMinSliderZ;
    QSlider *axisMaxSliderZ;
    QPushButton* calcPB;

    void InitMainLayout();
    void InitLogs();
    void InitParams();
    void InitMethods();
    void InitRenderFormat();
    void InitCalcButton();
};

#endif // SURFACEVIEW_H
