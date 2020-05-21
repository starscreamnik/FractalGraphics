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

#include "vector3.h"
#include "vector2.h"

using namespace std;
using namespace QtConcurrent;

struct ThreadWorkInput {
    Vector3** data;
    unsigned int rowFrom;
    unsigned int rowTo;
    unsigned int numSquares;
    unsigned int squareSize;
    double height;
};

struct DiamondSquareTaskInput{
    Vector3** data;
    unsigned int row;
    unsigned int col;
    unsigned int numSquares;
    unsigned int squareSize;
    double height;
};

class DiamondSquare {
public:
    DiamondSquare(const unsigned int divisions, const double size, const double height);
    ~DiamondSquare();
    void CreateHeightMap();
    void CreateHeightMapByThreads();
    void WriteHeightMapToFile(const QString filePath);

private:
    unsigned int mDivisions;
    double mSize;
    double mHeight;

    Vector3** mVerts;
    Vector2** uvs;
    vector<int> tris;
    int mVertCount;

    static void ThreadWork(const ThreadWorkInput& task);
    static void DiamondSquareThreadTask(const DiamondSquareTaskInput& task);

    void DiamondSquareTask(unsigned int row, unsigned int col, unsigned int size, double offset);
    bool IsPowerOfTwo(unsigned int x);
    unsigned int GetPowerOfTwoUpper(unsigned int x);
    static double GetRandomDoubleInRange(double from, double to);
};

