#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtCore/qmath.h>

const int sampleCountX = 100;
const int sampleCountZ = 100;
const int heightMapGridStepX = 10;
const int heightMapGridStepZ = 10;
const float sampleMin = -100.0f;
const float sampleMax = 100.0f;

SurfaceGraph::SurfaceGraph(Q3DSurface *surface, unsigned int divisions, float size, float height, QListWidget* logs):
    heightMapGenerator(nullptr),
    heightmapDivisions(divisions),
    heightmapSize(size),
    heightmapYAmplitude(height),
    graph(surface),
    series(nullptr),
    logs(logs)
{
    graph->setAxisX(new QValue3DAxis);
    graph->setAxisY(new QValue3DAxis);
    graph->setAxisZ(new QValue3DAxis);

    setAxisYRange(-100, 100);
    setAxisXRange(-100, 100);
    setAxisZRange(-100, 100);
}

SurfaceGraph::~SurfaceGraph()
{
    delete graph;
}

void SurfaceGraph::calculate(){
    if (heightMapGenerator != nullptr) {
        delete heightMapGenerator;
        heightMapGenerator = nullptr;
    }

    if (series != nullptr){
        graph->removeSeries(series);
        delete series;
        series = nullptr;
    }

    heightMapGenerator = new DiamondSquare(heightmapDivisions, heightmapSize, heightmapYAmplitude);

    QTime time;
    time.start();
    (heightMapGenerator->*methodPointer)();

    logs->addItem(methodName + ": " + QString::number(static_cast<double>(time.elapsed()/1000.0)));

    auto proxy = new QSurfaceDataProxy;
    proxy->resetArray(heightMapGenerator->GetData());

    series = new QSurface3DSeries(proxy);
    series->setDrawMode(QSurface3DSeries::DrawSurface);
    series->setFlatShadingEnabled(true);

    graph->addSeries(series);

    setGradient();

    m_rangeMinX = sampleMin;
    m_rangeMinZ = sampleMin;
    m_stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
    m_stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);
    m_axisMinSliderX->setMaximum(sampleCountX - 2);
    m_axisMinSliderX->setValue(0);
    m_axisMaxSliderX->setMaximum(sampleCountX - 1);
    m_axisMaxSliderX->setValue(sampleCountX - 1);
    m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
    m_axisMinSliderZ->setValue(0);
    m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
    m_axisMaxSliderZ->setValue(sampleCountZ - 1);
}

void SurfaceGraph::setAxisXRange(float min, float max)
{
    graph->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisYRange(float min, float max)
{
    graph->axisY()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(float min, float max)
{
    graph->axisZ()->setRange(min, max);
}


void SurfaceGraph::adjustXMin(int min)
{
    float minX = m_stepX * float(min) + m_rangeMinX;

    int max = m_axisMaxSliderX->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderX->setValue(max);
    }
    float maxX = m_stepX * max + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
    float maxX = m_stepX * float(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderX->setValue(min);
    }
    float minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
    float minZ = m_stepZ * float(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderZ->setValue(max);
    }
    float maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
    float maxX = m_stepZ * float(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderZ->setValue(min);
    }
    float minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

void SurfaceGraph::setGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::blue);
    gr.setColorAt(0.25, Qt::green);
    gr.setColorAt(0.45, Qt::green);
    gr.setColorAt(0.45, Qt::yellow);
    gr.setColorAt(0.9, Qt::white);

    graph->seriesList().at(0)->setBaseGradient(gr);
    graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
