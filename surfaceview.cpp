#include "surfaceview.h"

SurfaceView::SurfaceView()
{
    graph = new Q3DSurface();
    container = QWidget::createWindowContainer(graph);
    widget = new QWidget;

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    InitMainLayout();
    InitLogs();
    InitMethods();
    InitParams();
    InitRenderFormat();
    InitCalcButton();

    widget->setWindowTitle(QStringLiteral("Fractal Graphics"));
    widget->show();

    modifier = new SurfaceController(graph, 64, 100.f, 10.f, logs);

    QObject::connect(sequenceRB, &QRadioButton::toggled,
                     modifier, &SurfaceController::toggleSequenceMethod);
    QObject::connect(parallelRB, &QRadioButton::toggled,
                     modifier, &SurfaceController::toggleQTConcurrentMethod);

    QObject::connect(divisionsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     modifier, &SurfaceController::setSurfaceDivisions);

    QObject::connect(sizeSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     modifier, &SurfaceController::setSurfaceSize);

    QObject::connect(heightSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     modifier, &SurfaceController::setSurfaceHeight);

    QObject::connect(axisMinSliderX, &QSlider::valueChanged,
                     modifier, &SurfaceController::adjustXMin);
    QObject::connect(axisMaxSliderX,  &QSlider::valueChanged,
                     modifier, &SurfaceController::adjustXMax);
    QObject::connect(axisMinSliderZ,  &QSlider::valueChanged,
                     modifier, &SurfaceController::adjustZMin);
    QObject::connect(axisMaxSliderZ,  &QSlider::valueChanged,
                     modifier, &SurfaceController::adjustZMax);

    QObject::connect(calcPB, &QPushButton::released,
                     modifier, &SurfaceController::calculate);

    sequenceRB->setChecked(true);
}

SurfaceView::~SurfaceView(){
    delete graph;
};

void SurfaceView::InitMainLayout(){
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    hLayout->addWidget(container, 1);

    settingsLayout = new QVBoxLayout;
    QGroupBox* settingsGroup = new QGroupBox();
    settingsGroup->setLayout(settingsLayout);

    hLayout->addWidget(settingsGroup);
}

void SurfaceView::InitLogs(){
    QGroupBox *logGroup = new QGroupBox(QStringLiteral("Calculation Time Log"));
    logs = new QListWidget;

    QVBoxLayout *logLayout = new QVBoxLayout;
    logLayout->addWidget(logs);
    logGroup->setLayout(logLayout);

    settingsLayout->addWidget(logGroup);
}

void SurfaceView::InitParams(){
    QGroupBox *paramsGroup = new QGroupBox(QStringLiteral("Params"));

    QLabel* divLabel = new QLabel("Divisions");
    divisionsSB = new QSpinBox(paramsGroup);
    divisionsSB->setValue(64);
    divisionsSB->setMinimum(2);
    divisionsSB->setMaximum(8192);


    QLabel* sizeLabel = new QLabel("Size");
    sizeSB = new QDoubleSpinBox(paramsGroup);
    sizeSB->setValue(100.0);
    sizeSB->setMinimum(0.);
    sizeSB->setMaximum(1000.);

    QLabel* heightLabel = new QLabel("Height");
    heightSB = new QDoubleSpinBox(paramsGroup);
    heightSB->setValue(10.);
    heightSB->setMinimum(0.);
    heightSB->setMaximum(1000.);

    QGridLayout* paramsLayout = new QGridLayout;
    paramsLayout->addWidget(divLabel, 0, 0);
    paramsLayout->addWidget(divisionsSB, 0, 1);

    paramsLayout->addWidget(sizeLabel, 1, 0);
    paramsLayout->addWidget(sizeSB, 1, 1);

    paramsLayout->addWidget(heightLabel, 2, 0);
    paramsLayout->addWidget(heightSB, 2, 1);

    paramsGroup->setLayout(paramsLayout);

    settingsLayout->addWidget(paramsGroup);
}

void SurfaceView::InitMethods(){
    QGroupBox *methodsGroup = new QGroupBox(QStringLiteral("Method"));

    sequenceRB = new QRadioButton(widget);
    sequenceRB->setText(QStringLiteral("Sequence"));
    sequenceRB->setChecked(false);

    parallelRB = new QRadioButton(widget);
    parallelRB->setText(QStringLiteral("QTConcurrent"));
    parallelRB->setChecked(false);

    QVBoxLayout *methodsLayout = new QVBoxLayout;
    methodsLayout->addWidget(sequenceRB);
    methodsLayout->addWidget(parallelRB);
    methodsGroup->setLayout(methodsLayout);

    settingsLayout->addWidget(methodsGroup);
}

void SurfaceView::InitRenderFormat(){
    QGroupBox *formatsGroup = new QGroupBox(QStringLiteral("Render Format"));

    QLabel* colLabel = new QLabel("Column Range");

    axisMinSliderX = new QSlider(Qt::Horizontal, widget);
    axisMinSliderX->setMinimum(0);
    axisMinSliderX->setTickInterval(1);
    axisMinSliderX->setEnabled(true);
    axisMaxSliderX = new QSlider(Qt::Horizontal, widget);
    axisMaxSliderX->setMinimum(1);
    axisMaxSliderX->setTickInterval(1);
    axisMaxSliderX->setEnabled(true);

    QLabel* rowLabel = new QLabel("Row Range");

    axisMinSliderZ = new QSlider(Qt::Horizontal, widget);
    axisMinSliderZ->setMinimum(0);
    axisMinSliderZ->setTickInterval(1);
    axisMinSliderZ->setEnabled(true);
    axisMaxSliderZ = new QSlider(Qt::Horizontal, widget);
    axisMaxSliderZ->setMinimum(1);
    axisMaxSliderZ->setTickInterval(1);
    axisMaxSliderZ->setEnabled(true);

    QGridLayout* formatsLayout = new QGridLayout;
    formatsLayout->addWidget(colLabel, 0, 0);
    formatsLayout->addWidget(axisMinSliderX, 0, 1);
    formatsLayout->addWidget(axisMaxSliderX, 1, 1);

    formatsLayout->addWidget(rowLabel, 2, 0);
    formatsLayout->addWidget(axisMinSliderZ, 2, 1);
    formatsLayout->addWidget(axisMaxSliderZ, 3, 1);
    formatsGroup->setLayout(formatsLayout);

    settingsLayout->addWidget(formatsGroup);
}

void SurfaceView::InitCalcButton(){
    calcPB = new QPushButton("Calculate");
    settingsLayout->addWidget(calcPB);
}

Q3DSurface* SurfaceView::GetGraph(){
    return graph;
}

QListWidget* SurfaceView::GetLogs(){
    return logs;
}

