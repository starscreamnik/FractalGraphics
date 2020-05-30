#include "diamondsquare.h"

using namespace QtDataVisualization;

DiamondSquare::DiamondSquare
(
    const int divisions, const float size, const float height
) {
    mDivisions = IsPowerOfTwo(divisions) ? divisions : GetPowerOfTwoUpper(divisions);
    mSize = size >= 1.0f ? size : 10.0f;
    mHeight = height >= 1.0f ? height : 10.0f;

    Init();
}

DiamondSquare::DiamondSquare():mDivisions(0),mSize(0),mHeight(0){
    qDebug() << "DiamondSquare() constructor" << endl;
}

DiamondSquare::~DiamondSquare() {}


void DiamondSquare::Init(){
    mVerts = new QVector3D*[mDivisions + 1];

    float halfSize = mSize / 2;
    float divisionSize = mSize / static_cast<float>(mDivisions);

    for (int i = 0; i <= mDivisions; i++) {
        mVerts[i] = new QVector3D[mDivisions + 1];
        for (int j = 0; j <= mDivisions; j++) {
            mVerts[i][j].setX(-halfSize + j * divisionSize);
            mVerts[i][j].setY(0.0f);
            mVerts[i][j].setZ(halfSize - i * divisionSize);
        }
    }
}

void DiamondSquare::Resize(const int divisions, const float size, const float height){
    for (int i = 0; i <= mDivisions; i++){
        delete[] mVerts[i];
    }
    delete[] mVerts;
    mVerts = nullptr;

    mDivisions = IsPowerOfTwo(divisions) ? divisions : GetPowerOfTwoUpper(divisions);
    mSize = size >= 1.0f ? size : 10.0f;
    mHeight = height >= 1.0f ? height : 10.0f;

    Init();
}

QSurfaceDataArray* DiamondSquare::GetData()const{
    QSurfaceDataArray* data = new QSurfaceDataArray;
    QSurfaceDataRow* dataRow;

    for (int i = 0; i < mDivisions+1; i++){
        dataRow = new QSurfaceDataRow;
        for (int j = 0; j < mDivisions+1; j++){
            *dataRow << mVerts[i][j];
        }
        *data << dataRow;
        dataRow = nullptr;
    }

    return data;
}

void DiamondSquare::CreateHeightMap() {
    qDebug() << "DiamondSquare:CreateHeightMap -> run" << endl;

    mVerts[0][0]                    .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[0][mDivisions]           .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][0]           .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][mDivisions]  .setY(GetRandomFloatInRange(-mHeight, mHeight));

    int iterations = static_cast<int>(log2(mDivisions));
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
        mHeight *= 0.5f;
    }
}

void DiamondSquare::DiamondSquareTask(
    int row, int col, int size, float offset
) {
    int halfSize = size / 2;
    int midRow = row + halfSize;
    int midCol = col + halfSize;
    int lastRow = row + size;
    int lastCol = col + size;

    mVerts[midRow][midCol].setY
    (
        (   mVerts[row][col].y() +
            mVerts[row][lastCol].y() +
            mVerts[lastRow][col].y() +
            mVerts[lastRow][lastCol].y()
        ) * 0.25f + GetRandomFloatInRange(-offset, offset)
    );

    mVerts[row][midCol].setY( (mVerts[row][col].y() + mVerts[row][lastCol].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[midRow][col].setY( (mVerts[row][col].y() + mVerts[lastRow][col].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[midRow][lastCol].setY( (mVerts[row][lastCol].y() + mVerts[lastRow][lastCol].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[lastRow][midCol].setY( (mVerts[lastRow][col].y() + mVerts[lastRow][lastCol].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
}

void DiamondSquare::CreateHeightMapByThreads(){
    qDebug() << "DiamondSquare:CreateHeightMapByThreads -> run" << endl;

    mVerts[0][0]                      .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[0][mDivisions]             .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][0]             .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][mDivisions]    .setY(GetRandomFloatInRange(-mHeight, mHeight));

    static const int THREAD_COUNT = static_cast<int>(QThread::idealThreadCount());
    qDebug() << "Thread count:" << THREAD_COUNT << endl;


    int iterations = static_cast<int>(log2(mDivisions));
    int numSquares = 1;
    int squareSize = mDivisions;
    QVector<ThreadWorkInput> tasks;

    for (int i = 0; i < iterations; i++) {
        int dy = numSquares < THREAD_COUNT? 1: numSquares/THREAD_COUNT;

        for(int y = 0; y < numSquares; y += dy ) {
           tasks << ThreadWorkInput { mVerts, y, y + dy, numSquares, squareSize, mHeight };
        }

        QFuture<void> future = QtConcurrent::map( tasks, ThreadWork );
        future.waitForFinished();

        numSquares *= 2;
        squareSize /= 2;
        mHeight *= 0.5f;
        tasks.clear();
    }
}

void DiamondSquare::ThreadWork(const ThreadWorkInput& task){
    int row = task.rowFrom * task.squareSize;
    auto dsTask = DiamondSquareTaskInput();
    dsTask.data = task.data;
    dsTask.height = task.height;
    dsTask.squareSize = task.squareSize;
    dsTask.numSquares = task.numSquares;

    for (int i = task.rowFrom; i < task.rowTo; i++) {
        int col = 0;

        for (int j = 0; j < task.numSquares; j++) {
            dsTask.row = row;
            dsTask.col = col;
            DiamondSquareThreadTask(dsTask);

            col += task.squareSize;
        }
        row += task.squareSize;
    }
}

void DiamondSquare::DiamondSquareThreadTask(const DiamondSquareTaskInput& task) {
    int size = task.squareSize;
    float offset = task.height;

    int halfSize = size / 2;
    int row = task.row;
    int col = task.col;
    int midRow = row + halfSize;
    int midCol = col + halfSize;
    int lastRow = row + size;
    int lastCol = col + size;

    task.data[midRow][midCol].setY
    (
        (   task.data[row][col].y() +
            task.data[row][lastCol].y() +
            task.data[lastRow][col].y() +
            task.data[lastRow][lastCol].y()
        ) * 0.25f + GetRandomFloatInRange(-offset, offset)
    );

    task.data[row][midCol].setY( (task.data[row][col].y() + task.data[row][lastCol].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[midRow][col].setY( (task.data[row][col].y() + task.data[lastRow][col].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[midRow][lastCol].setY( (task.data[row][lastCol].y() + task.data[lastRow][lastCol].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[lastRow][midCol].setY( (task.data[lastRow][col].y() + task.data[lastRow][lastCol].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
}

bool DiamondSquare::IsPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int DiamondSquare::GetPowerOfTwoUpper(int x) {
    return static_cast<int>(pow(2, ceil(log(x) / log(2))));
}

float DiamondSquare::GetRandomFloatInRange(float from, float to) {
    float f = static_cast<float>(rand()) / RAND_MAX;
    return from + f * (to - from);
}

int DiamondSquare::GetSize()const{
    return mDivisions;
}
