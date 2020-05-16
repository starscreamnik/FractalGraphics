#pragma once
#include <math.h>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <fstream>
#include "vector3.h"
#include "vector2.h"

using namespace std;

class DiamondSquare {
public:
    DiamondSquare(int divisions, int size, double height);
    ~DiamondSquare();
    void CreateHeightMap();
    void WriteHeightMapToFile(ofstream& out);

private:
    int mDivisions;
    int mSize;
    double mHeight;

    vector<vector<Vector3>> mVerts;
    vector<vector<Vector2>> uvs;
    vector<int> tris;
    int mVertCount;

    void DiamondSquareTask(int row, int col, int size);
    bool IsPowerOfTwo(int x);
    int GetPowerOfTwoUpper(unsigned int x);
    int GetRandomIntInRange(double from, double to);
};

