#pragma once
#include <math.h>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QFutureWatcher>
#include <iostream>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataRow>
#include <QtDataVisualization/QSurfaceDataArray>

#include "vector3.h"

using namespace std;
using namespace QtConcurrent;
using namespace QtDataVisualization;

struct ThreadWorkInput {
    QVector3D** data;
    unsigned int rowFrom;
    unsigned int rowTo;
    unsigned int numSquares;
    unsigned int squareSize;
    float height;
};

struct DiamondSquareTaskInput{
    QVector3D** data;
    unsigned int row;
    unsigned int col;
    unsigned int numSquares;
    unsigned int squareSize;
    float height;
};

class DiamondSquare {
public:
    DiamondSquare(const unsigned int divisions, const float size, const float height);
    ~DiamondSquare();
    void CreateHeightMap();
    void CreateHeightMapByThreads();
    QSurfaceDataArray* GetData();
    void WriteHeightMapToFile(const QString filePath);
    u_int GetSize()const;

    typedef unsigned int u_int;

private:
    unsigned int mDivisions;
    float mSize;
    float mHeight;
    bool IsPowerOfTwo(unsigned int x);
    unsigned int GetPowerOfTwoUpper(unsigned int x);

    QVector3D** mVerts;

    void init();
    static float GetRandomFloatInRange(float from, float to);

    void DiamondSquareTask(unsigned int row, unsigned int col, unsigned int size, float offset);

    static void ThreadWork(const ThreadWorkInput& task);
    static void DiamondSquareThreadTask(const DiamondSquareTaskInput& task);

};

