#include "diamondsquare.h"

using namespace QtDataVisualization;

DiamondSquare::DiamondSquare
(
    const unsigned int divisions, const float size, const float height
) {
    mDivisions = IsPowerOfTwo(divisions) ? divisions : GetPowerOfTwoUpper(divisions);
    mSize = size >= 1.0f ? size : 10.0f;
    mHeight = height >= 1.0f ? height : 10.0f;

    init();
}

DiamondSquare::~DiamondSquare() {}


void DiamondSquare::init(){
    mVerts = new QVector3D*[mDivisions + 1];

    float halfSize = mSize / 2;
    float divisionSize = mSize / static_cast<float>(mDivisions);

    for (unsigned int i = 0; i <= mDivisions; i++) {
        mVerts[i] = new QVector3D[mDivisions + 1];
        for (unsigned int j = 0; j <= mDivisions; j++) {
            mVerts[i][j].setX(-halfSize + j * divisionSize);
            mVerts[i][j].setY(0.0f);
            mVerts[i][j].setZ(halfSize - i * divisionSize);
        }
    }
}

QSurfaceDataArray* DiamondSquare::GetData(){
    QSurfaceDataArray* data = new QSurfaceDataArray;
    QSurfaceDataRow* dataRow;

    for (u_int i = 0; i < mDivisions+1; i++){
        dataRow = new QSurfaceDataRow;
        for (u_int j = 0; j < mDivisions+1; j++){
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
    unsigned int squareSize = mDivisions;
    for (int i = 0; i < iterations; i++) {

        unsigned int row = 0;
        for (int j = 0; j < numSquares; j++) {

            unsigned int col = 0;
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
    unsigned int row, unsigned int col, unsigned int size, float offset
) {
    unsigned int halfSize = size / 2;

    unsigned int topLeftRow = row;
    unsigned int topLeftCol = col;

    unsigned int botLeftRow = row + size;
    unsigned int botLeftCol = col;

    unsigned int midRow = row + halfSize;
    unsigned int midCol = col + halfSize;

    mVerts[midRow][midCol].setY
    (
        (   mVerts[topLeftRow][topLeftCol].y() +
            mVerts[topLeftRow][topLeftCol + size].y() +
            mVerts[topLeftRow][topLeftCol].y() +
            mVerts[botLeftRow][botLeftCol + size].y()
        ) * 0.25f + GetRandomFloatInRange(-offset, offset)
    );

    mVerts[topLeftRow][topLeftCol + halfSize]   .setY( (mVerts[topLeftRow][topLeftCol].y() + mVerts[topLeftRow][topLeftCol + size].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[midRow][midCol - halfSize]           .setY( (mVerts[topLeftRow][topLeftCol].y() + mVerts[botLeftRow][botLeftCol].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[midRow][midCol + halfSize]           .setY( (mVerts[topLeftRow][topLeftCol + size].y() + mVerts[botLeftRow][botLeftCol + size].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    mVerts[botLeftRow][botLeftCol + halfSize]   .setY( (mVerts[botLeftRow][botLeftCol].y() + mVerts[botLeftRow][botLeftCol + size].y() + mVerts[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
}

void DiamondSquare::CreateHeightMapByThreads(){
    qDebug() << "DiamondSquare:CreateHeightMapByThreads -> run" << endl;

    mVerts[0][0]                      .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[0][mDivisions]             .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][0]             .setY(GetRandomFloatInRange(-mHeight, mHeight));
    mVerts[mDivisions][mDivisions]    .setY(GetRandomFloatInRange(-mHeight, mHeight));

    static const unsigned int THREAD_COUNT = static_cast<unsigned int>(QThread::idealThreadCount());
    qDebug() << "Thread count:" << THREAD_COUNT << endl;

    unsigned int iterations = static_cast<unsigned int>(log2(mDivisions));
    unsigned int numSquares = 1;
    unsigned int squareSize = mDivisions;
    QVector<ThreadWorkInput> tasks;

    for (unsigned int i = 0; i < iterations; i++) {
        unsigned int dy = numSquares < THREAD_COUNT? 1: numSquares/THREAD_COUNT;

        for(unsigned int y = 0; y < numSquares; y += dy ) {
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
    unsigned int row = task.rowFrom * task.squareSize;
    auto dsTask = DiamondSquareTaskInput();
    dsTask.data = task.data;
    dsTask.height = task.height;
    dsTask.squareSize = task.squareSize;
    dsTask.numSquares = task.numSquares;

    for (unsigned int i = task.rowFrom; i < task.rowTo; i++) {
        unsigned int col = 0;

        for (unsigned int j = 0; j < task.numSquares; j++) {
            dsTask.row = row;
            dsTask.col = col;
            DiamondSquareThreadTask(dsTask);

            col += task.squareSize;
        }
        row += task.squareSize;
    }
}

void DiamondSquare::DiamondSquareThreadTask(const DiamondSquareTaskInput& task) {
    unsigned int halfSize = task.squareSize / 2;
    unsigned int size = task.squareSize;
    float offset = task.height;

    unsigned int topLeftRow = task.row;
    unsigned int topLeftCol = task.col;

    unsigned int botLeftRow = task.row + size;
    unsigned int botLeftCol = task.col;

    unsigned int midRow = task.row + halfSize;
    unsigned int midCol = task.col + halfSize;

    task.data[midRow][midCol].setY
    (
        (   task.data[topLeftRow][topLeftCol].y() +
            task.data[topLeftRow][topLeftCol + size].y() +
            task.data[topLeftRow][topLeftCol].y() +
            task.data[botLeftRow][botLeftCol + size].y()
        ) * 0.25f + GetRandomFloatInRange(-offset, offset)
    );

    task.data[topLeftRow][topLeftCol + halfSize].setY( (task.data[topLeftRow][topLeftCol].y() + task.data[topLeftRow][topLeftCol + size].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[midRow][midCol - halfSize]        .setY( (task.data[topLeftRow][topLeftCol].y() + task.data[botLeftRow][botLeftCol].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[midRow][midCol + halfSize]        .setY( (task.data[topLeftRow][topLeftCol + size].y() + task.data[botLeftRow][botLeftCol + size].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
    task.data[botLeftRow][botLeftCol + halfSize].setY( (task.data[botLeftRow][botLeftCol].y() + task.data[botLeftRow][botLeftCol + size].y() + task.data[midRow][midCol].y()) / 3 + GetRandomFloatInRange(-offset, offset) );
}

void DiamondSquare::WriteHeightMapToFile(const QString filePath) {
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        // We're going to streaming text to the file
        QTextStream out(&file);

        out << mDivisions+1 << endl;

        for (unsigned int i=0; i < (mDivisions+1); i++) {
            out << endl;
            for (unsigned int j=0; j < (mDivisions+1); j++) {
                auto item = mVerts[i][j];
                out << item.x() << " " << item.y() << " " << item.z() << endl;
            }
        }

        file.close();
        qDebug() << "WriteHeigthtMapToFile: Writing finished";
    }
}

bool DiamondSquare::IsPowerOfTwo(unsigned int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

unsigned int DiamondSquare::GetPowerOfTwoUpper(unsigned int x) {
    return static_cast<unsigned int>(pow(2, ceil(log(x) / log(2))));
}

float DiamondSquare::GetRandomFloatInRange(float from, float to) {
    float f = static_cast<float>(rand()) / RAND_MAX;
    return from + f * (to - from);
}

unsigned int DiamondSquare::GetSize()const{
    return mDivisions;
}
