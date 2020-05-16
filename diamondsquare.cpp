#include "diamondsquare.h"

DiamondSquare::DiamondSquare(int divisions, int size, double height) {
    mSize = IsPowerOfTwo(size) ? size : GetPowerOfTwoUpper(size);
    mDivisions = divisions > 0 ? divisions : mSize / 3;
    mHeight = height > 0 ? height : 100;

    printf("size: %d->%d,\ndivisions: %d->%d,\nheight: %f->%f\n",
        size, mSize, divisions, mDivisions, height, mHeight);

    //mVertCount = (mDivisions + 1) * (mDivisions + 1);
    mVerts.resize(mDivisions+1);
    uvs.resize(mDivisions+1);
    tris.resize(mDivisions * mDivisions * 6);

    printf("mVertCount: %d,\nmVerts.size: %d,\nuvs.size: %d,\ntris.size: %d\n",
        mVertCount, mVerts.size(), uvs.size(), tris.size());

    double halfSize = mSize / 2;
    double divisionSize = (double)mSize / (double)mDivisions;
    int trisOffset = 0;

    for (int i = 0; i <= mDivisions; i++) {
        mVerts[i].resize(mDivisions + 1);
        uvs[i].resize(mDivisions + 1);
        for (int j = 0; j <= mDivisions; j++) {
            mVerts[i][j].x = -halfSize + j * divisionSize;
            mVerts[i][j].y = 0.0;
            mVerts[i][j].z = halfSize - i * divisionSize;
            uvs[i][j].x = i / mDivisions;
            uvs[i][j].y = j / mDivisions;
            if (i < mDivisions && j < mDivisions) {
                int topLeft = i * (mDivisions + 1) + j;
                int botLeft = (i + 1) * (mDivisions + 1) + j;
                tris[trisOffset] = topLeft;
                tris[trisOffset + 1] = topLeft + 1;
                tris[trisOffset + 2] = botLeft + 1;
                tris[trisOffset + 3] = topLeft;
                tris[trisOffset + 4] = botLeft + 1;
                tris[trisOffset + 5] = botLeft;

                trisOffset += 6;
            }
        }
    }
}

DiamondSquare::~DiamondSquare() {
}

bool DiamondSquare::IsPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int DiamondSquare::GetPowerOfTwoUpper(unsigned int x) {
    return pow(2, ceil(log(x) / log(2)));
}

int DiamondSquare::GetRandomIntInRange(double from, double to) {
    return rand() % (int)ceil(to) + (int)floor(from);
}

void DiamondSquare::WriteHeightMapToFile(ofstream& out) {
    out << mVerts.size() << endl;

    for (auto& row: mVerts) {
        out << endl;
        for (auto& col : row) {
            out << round(col.x) << " " << round(col.y) << " " << round(col.z) << endl;
        }
    }
}

void DiamondSquare::CreateHeightMap() {
    mVerts[0][0].y                      = GetRandomIntInRange(-mHeight, mHeight);
    mVerts[0][mDivisions].y             = GetRandomIntInRange(-mHeight, mHeight);
    mVerts[mDivisions][0].y             = GetRandomIntInRange(-mHeight, mHeight);
    mVerts[mDivisions][mDivisions].y    = GetRandomIntInRange(-mHeight, mHeight);

    int iterations = (int)log2(mDivisions);
    int numSquares = 1;
    int squareSize = mDivisions;
    for (int i = 0; i < iterations; i++) {

        int row = 0;
        for (int j = 0; j < numSquares; j++) {

            int col = 0;
            for (int k = 0; k < numSquares; k++) {

                DiamondSquareTask(row, col, squareSize);
                col += squareSize;
            }
            row += squareSize;
        }
        numSquares *= 2;
        squareSize /= 2;
        mHeight *= 0.5;
    }
}

void DiamondSquare::DiamondSquareTask(int row, int col, int size) {
    int halfSize = (int)(size * 0.5);

    int topLeft = row * (mDivisions + 1) + col;
    int topLeftRow = row;
    int topLeftCol = col;

    int botLeft = (row + size) * (mDivisions + 1) + col;
    int botLeftRow = row + size;
    int botLeftCol = col;

    int mid = (int)(row + halfSize) * (mDivisions + 1) + (int)(col + halfSize);
    int midRow = (int)(row + halfSize);
    int midCol = (int)(col + halfSize);

    mVerts[midRow][midCol].y                        = (mVerts[topLeftRow][topLeftCol].y + mVerts[topLeftRow][topLeftCol + size].y + mVerts[topLeftRow][topLeftCol].y + mVerts[botLeftRow][botLeftCol + size].y) * 0.25 + GetRandomIntInRange(-mHeight, mHeight);

    mVerts[topLeftRow][topLeftCol + halfSize].y     = (mVerts[topLeftRow][topLeftCol].y + mVerts[topLeftRow][topLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomIntInRange(-mHeight, mHeight);
    mVerts[midRow][midCol - halfSize].y             = (mVerts[topLeftRow][topLeftCol].y + mVerts[botLeftRow][botLeftCol].y + mVerts[midRow][midCol].y) / 3 + GetRandomIntInRange(-mHeight, mHeight);
    mVerts[midRow][midCol + halfSize].y             = (mVerts[topLeftRow][topLeftCol + size].y + mVerts[botLeftRow][botLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomIntInRange(-mHeight, mHeight);
    mVerts[botLeftRow][botLeftCol + halfSize].y     = (mVerts[botLeftRow][botLeftCol].y + mVerts[botLeftRow][botLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomIntInRange(-mHeight, mHeight);
}
