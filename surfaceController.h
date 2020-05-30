#ifndef SURFACECONTROLLER_H
#define SURFACECONTROLLER_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include <QMainWindow>
#include <memory>
#include <QListWidget>
#include <QTime>

#include "diamondsquare.h"

using namespace QtDataVisualization;

class SurfaceController: public QMainWindow{
    Q_OBJECT

public:
    explicit SurfaceController(Q3DSurface *surface, int divisions, float size, float height, QListWidget* logs);
    ~SurfaceController();

    void toggleSequenceMethod()     { methodPointer = &DiamondSquare::CreateHeightMap; methodName="Sequence"; }
    void toggleQTConcurrentMethod() { methodPointer = &DiamondSquare::CreateHeightMapByThreads; methodName="QTConcurrent"; }

    void setSurfaceDivisions(int value) { heightmapDivisions = value; }
    void setSurfaceSize(double value)   { heightmapSize = static_cast<float>(value); }
    void setSurfaceHeight(double value) { heightmapYAmplitude = static_cast<float>(value); }

    void setAxisMinSliderX(QSlider *slider) { m_axisMinSliderX = slider; }
    void setAxisMaxSliderX(QSlider *slider) { m_axisMaxSliderX = slider; }
    void setAxisMinSliderZ(QSlider *slider) { m_axisMinSliderZ = slider; }
    void setAxisMaxSliderZ(QSlider *slider) { m_axisMaxSliderZ = slider; }

    void adjustXMin(int min);
    void adjustXMax(int max);
    void adjustZMin(int min);
    void adjustZMax(int max);

    void calculate();

private:
    DiamondSquare* heightMapGenerator;
    void (DiamondSquare::* methodPointer)();
    int heightmapDivisions;
    float heightmapSize;
    float heightmapYAmplitude;

    QString methodName;

    Q3DSurface* graph;
    QSurface3DSeries *series;
    QListWidget* logs;

    int m_heightMapWidth;
    int m_heightMapHeight;

    QSlider *m_axisMinSliderX;
    QSlider *m_axisMaxSliderX;
    QSlider *m_axisMinSliderZ;
    QSlider *m_axisMaxSliderZ;
    float m_rangeMinX;
    float m_rangeMinZ;
    float m_stepX;
    float m_stepZ;

    void setAxisXRange(float min, float max);
    void setAxisYRange(float min, float max);
    void setAxisZRange(float min, float max);

    void setGradient();

};

#endif // SURFACECONTROLLER_H
