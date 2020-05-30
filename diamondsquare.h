#pragma once
#include <cmath>
#include <cstdlib>
#include <QDebug>
#include <QThread>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtDataVisualization/QSurfaceDataRow>
#include <QtDataVisualization/QSurfaceDataArray>


using namespace std;
using namespace QtConcurrent;
using namespace QtDataVisualization;

typedef QVector3D** DataSet;

struct ThreadWorkInput {
    DataSet data;
    int rowFrom;
    int rowTo;
    int numSquares;
    int squareSize;
    float height;
};

struct DiamondSquareTaskInput{
    DataSet data;
    int row;
    int col;
    int numSquares;
    int squareSize;
    float height;
};

class DiamondSquare {
public:
    DiamondSquare(const int divisions, const float size, const float height);
    DiamondSquare();
    ~DiamondSquare();
    void CreateHeightMap();
    void CreateHeightMapByThreads();
    QSurfaceDataArray* GetData()const;
    int GetSize()const;
    void Resize(const int divisions, const float size, const float height);

private:
    int mDivisions;
    float mSize;
    float mHeight;
    bool IsPowerOfTwo(int x);
    int GetPowerOfTwoUpper(int x);

    DataSet mVerts;

    void Init();
    static float GetRandomFloatInRange(float from, float to);

    void DiamondSquareTask(int row, int col, int size, float offset);

    static void ThreadWork(const ThreadWorkInput& task);
    static void DiamondSquareThreadTask(const DiamondSquareTaskInput& task);

};

