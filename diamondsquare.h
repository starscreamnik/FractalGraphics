#pragma once
#include <math.h>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <QFile>
#include <QTextStream>
#include<QDebug>
#include "vector3.h"
#include "vector2.h"

using namespace std;

class DiamondSquare {
public:
    DiamondSquare(const int divisions, const double size, const double height);
    ~DiamondSquare();
    void CreateHeightMap();
    void WriteHeightMapToFile(const QString filePath);

private:
    int mDivisions;
    double mSize;
    double mHeight;

    vector<vector<Vector3>> mVerts;
    vector<vector<Vector2>> uvs;
    vector<int> tris;
    int mVertCount;

    void DiamondSquareTask(int row, int col, int size, double offset);
    bool IsPowerOfTwo(int x);
    int GetPowerOfTwoUpper(unsigned int x);
    double GetRandomDoubleInRange(double from, double to);
};

