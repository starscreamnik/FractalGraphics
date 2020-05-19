#include "diamondsquare.h"

DiamondSquare::DiamondSquare(const int divisions, const double size, const double height) {
    mDivisions = IsPowerOfTwo(divisions) ? divisions : GetPowerOfTwoUpper(divisions);
    mSize = size >= 1.0 ? size : 10.0;
    mHeight = height >= 1.0 ? height : 10.0;

    printf("size: %f->%f,\ndivisions: %d->%d,\nheight: %f->%f\n",
        size, mSize, divisions, mDivisions, height, mHeight);

    mVertCount = (mDivisions + 1) * (mDivisions + 1);
    mVerts.resize(mDivisions + 1);
    uvs.resize(mDivisions + 1);
    tris.resize(mDivisions * mDivisions * 6);

    printf("mVertCount: %d,\nmVerts.size: %d,\nuvs.size: %d,\ntris.size: %d\n",
        mVertCount, mVerts.size(), uvs.size(), tris.size());

    double halfSize = mSize / 2;
    double divisionSize = mSize / (double)mDivisions;
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

double DiamondSquare::GetRandomDoubleInRange(double from, double to) {
    double f = (double)rand() / RAND_MAX;
    return from + f * (to - from);
}

void DiamondSquare::CreateHeightMap() {
    mVerts[0][0].y                      = GetRandomDoubleInRange(-mHeight, mHeight);
    mVerts[0][mDivisions].y             = GetRandomDoubleInRange(-mHeight, mHeight);
    mVerts[mDivisions][0].y             = GetRandomDoubleInRange(-mHeight, mHeight);
    mVerts[mDivisions][mDivisions].y    = GetRandomDoubleInRange(-mHeight, mHeight);

    int iterations = (int)log2(mDivisions);
    int numSquares = 1;
    int squareSize = mDivisions;
    for (int i = 0; i < iterations; i++) {

        int row = 0;
        for (int j = 0; j < numSquares; j++) {

            int col = 0;
            for (int k = 0; k < numSquares; k++) {

                DiamondSquareTask(row, col, squareSize, mHeight);
                col += squareSize;
            }
            row += squareSize;
        }
        numSquares *= 2;
        squareSize /= 2;
        mHeight *= 0.5;
    }
}

void DiamondSquare::DiamondSquareTask(int row, int col, int size, double offset) {
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

    mVerts[midRow][midCol].y                        =
        (   mVerts[topLeftRow][topLeftCol].y +
            mVerts[topLeftRow][topLeftCol + size].y +
            mVerts[topLeftRow][topLeftCol].y +
            mVerts[botLeftRow][botLeftCol + size].y
        ) * 0.25 + GetRandomDoubleInRange(-offset, offset);

    mVerts[topLeftRow][topLeftCol + halfSize].y     = (mVerts[topLeftRow][topLeftCol].y + mVerts[topLeftRow][topLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomDoubleInRange(-offset, offset);
    mVerts[midRow][midCol - halfSize].y             = (mVerts[topLeftRow][topLeftCol].y + mVerts[botLeftRow][botLeftCol].y + mVerts[midRow][midCol].y) / 3 + GetRandomDoubleInRange(-offset, offset);
    mVerts[midRow][midCol + halfSize].y             = (mVerts[topLeftRow][topLeftCol + size].y + mVerts[botLeftRow][botLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomDoubleInRange(-offset, offset);
    mVerts[botLeftRow][botLeftCol + halfSize].y     = (mVerts[botLeftRow][botLeftCol].y + mVerts[botLeftRow][botLeftCol + size].y + mVerts[midRow][midCol].y) / 3 + GetRandomDoubleInRange(-offset, offset);
}

void DiamondSquare::WriteHeightMapToFile(const QString filePath) {
    QFile file(filePath);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            // We're going to streaming text to the file
            QTextStream out(&file);

            out << mVerts.size() << endl;

            for (auto& row: mVerts) {
                out << endl;
                for (auto& col : row) {
                    out << col.x << " " << col.y << " " << col.z << endl;
                }
            }

            file.close();
            qDebug() << "WriteHeigthtMapToFile: Writing finished";
        }
}
