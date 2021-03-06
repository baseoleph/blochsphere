// A Bloch sphere emulator program.
// Copyright (C) 2022 Vasiliy Stephanov <baseoleph@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "MainWindow.h"
#include "BlochDialog.h"
#include "src/quantum/Operator.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QString>
#include <QTime>
#include <QTimer>
#include <QToolBar>
#include <cassert>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createSphere();
    createActions();
    createMenu();
    createTopBar();
    createSideWidget();
    createOpQueWidget();

    tm = new QTimer(this);
    connect(tm, SIGNAL(timeout()), SLOT(slotTimer()));
    statusBar()->show();
}

void MainWindow::addVector(Vector *v, MapVectors &mp) {
    mp.insert(v, QVector<Sphere *>());

    for (auto &e : spheres) {
        e->addVector(v);
        mp[v].append(e);
    }
}

void MainWindow::addVector(Vector *v, MapVectors &mp, Sphere *sph) {
    mp.insert(v, QVector<Sphere *>());

    sph->addVector(v);
    mp[v].append(sph);
}

void MainWindow::removeVector(Vector *v, MapVectors &mp) {
    for (auto &e : mp[v]) {
        e->deleteVector(v);
        e->update();
    }
    mp.remove(v);
    v->~Vector();
}

void MainWindow::removeAllVectors(MapVectors &mp) {
    while (not mp.isEmpty()) {
        removeVector(mp.keys().last(), mp);
    }
}

void MainWindow::slotTimer() {
    bool isNowAnimate = false;
    foreach (auto e, topTabWid->findChildren<VectorWidget *>()) {
        isNowAnimate |= e->getVector()->isNowAnimate();
        if (e->getVector() != nullptr) {
            if (e->getVector()->isNowAnimate()) {
                e->getVector()->takeStep();
            }
            e->fillFieldsOfVector(e->getVector()->getSpike());
        }
    }

    if (not isNowAnimate) {
        if (isQueueAnimation) {
            opQueue.last()->setBackground(QBrush(Qt::white));
            opQueue.pop_back();
            if (not opQueue.isEmpty()) {
                opQueue.last()->setBackground(QBrush(Qt::red));
                curOperator = opQueue.last()->getOp();
                updateOp();
                slotApplyOp();
                isQueueAnimation = true;
            } else {
                stopTimer();
                curOperator = singleOperator;
            }
        } else if (isCircuitAnimation) {
            circuit->stepUp();
            if (circuit->getCurrentStep() >= circuit->getSizeOfSteps()) {
                isCircuitAnimation = false;
                vectorangle va = curOperator.vectorAngleDec();
                foreach (auto e, vectors.keys()) {
                    e->setRotateVector(QVector3D(va.x, va.y, va.z));
                }
                circuit->slotStop();
                stopTimer();
            } else {
                nextAnimStepCircuit();
            }
        } else {
            stopTimer();
        }
    }

    slotUpdateSpheres();
}

void MainWindow::createSphere() {
    controlWidget = new QWidget(this);
    setCentralWidget(controlWidget);
    controlLayout = new QVBoxLayout(controlWidget);
    sphereLayout = new QHBoxLayout(controlLayout->widget());
    auto sphereWidget = new QWidget(controlWidget);
    sphereWidget->setLayout(sphereLayout);

    for (int i = 0; i < 1; ++i) {
        for (int j = 1; j < 2; ++j) {
            spheres.append(new Sphere(controlWidget));
            sphereLayout->addWidget(spheres.last());
        }
    }
    circuit = new Circuit(this);
    connect(this, SIGNAL(signalAnimating(bool)), circuit, SLOT(slotParentAnimating(bool)));
    connect(circuit, SIGNAL(signalStartAnimation()), this, SLOT(slotStartCircuitMove()));
    emit signalAnimating(false);
    controlLayout->addWidget(sphereWidget);
    controlLayout->addWidget(circuit);
    controlWidget->setFocus();
}

void MainWindow::createActions() {
    aboutAct = new QAction("About program", this);
    connect(aboutAct, SIGNAL(triggered()), SLOT(slotAbout()));

    resetAct = new QAction("Reset", this);
    connect(resetAct, SIGNAL(triggered()), SLOT(slotReset()));
    clearAct = new QAction("Clear", this);
    connect(clearAct, SIGNAL(triggered()), SLOT(slotClear()));

    showTAct = new QAction("Show trace", this);
    showTAct->setCheckable(true);
    showTAct->setChecked(true);
    connect(showTAct, SIGNAL(triggered()), SLOT(slotShowTrace()));

    clearTAct = new QAction("Clear trace", this);
    connect(clearTAct, SIGNAL(triggered()), SLOT(slotClearTrace()));

    exitAct = new QAction("Exit", this);
    connect(exitAct, SIGNAL(triggered()), SLOT(close()));
}

void MainWindow::createMenu() {
    // auto mnuBar = new QMenuBar();
    // auto menuFile = new QMenu("File");
    // auto menuInfo = new QMenu("Info");

    auto mnuBar = new QMenuBar(this);
    auto menuFile = new QMenu("File", mnuBar);
    auto menuInfo = new QMenu("Info", mnuBar);

    menuFile->addSeparator();
    menuFile->addAction(exitAct);
    menuInfo->addAction(aboutAct);

    mnuBar->addMenu(menuFile);
    mnuBar->addMenu(menuInfo);

    this->setMenuBar(mnuBar);
}

void MainWindow::createTopBar() {
    auto qtb = new QToolBar("Tool bar", this);
    qtb->addAction(resetAct);
    qtb->addAction(clearAct);
    qtb->addSeparator();
    qtb->addAction(showTAct);
    qtb->addAction(clearTAct);

    colorComboBox = new QComboBox(qtb);
    colorComboBox->addItem("Red");
    colorComboBox->addItem("Green");
    colorComboBox->addItem("Blue");
    colorComboBox->addItem("Grey");
    colorComboBox->addItem("Yellow");
    colorComboBox->setCurrentIndex(3);

    connect(colorComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotTraceColor(int)));

    // qtb->addWidget(new QLabel("<center>Trace Color:"));
    qtb->addWidget(new QLabel("<center>Trace Color:", qtb));
    qtb->addWidget(colorComboBox);
    qtb->addSeparator();

    auto normalizeCheckBox = new QCheckBox(qtb);
    normalizeCheckBox->setText("Auto normalize vectors");
    normalizeCheckBox->setChecked(true);

    connect(normalizeCheckBox, SIGNAL(toggled(bool)), SLOT(slotToggleAutoNormalize(bool)));

    qtb->addWidget(normalizeCheckBox);
    qtb->addSeparator();

    // speedLabel = new QLabel("Speed: 5 ");
    speedLabel = new QLabel("Speed: 5 ", qtb);
    qtb->addWidget(speedLabel);
    // speedUpButton = new QPushButton("Up");
    speedUpButton = new QPushButton("Up", qtb);
    speedUpButton->setFixedSize(50, 30);
    // speedDownButton = new QPushButton("Down");
    speedDownButton = new QPushButton("Down", qtb);
    speedDownButton->setFixedSize(50, 30);
    qtb->addWidget(speedUpButton);
    qtb->addWidget(speedDownButton);

    connect(speedUpButton, SIGNAL(clicked()), this, SLOT(slotSpeedUp()));
    connect(speedDownButton, SIGNAL(clicked()), this, SLOT(slotSpeedDown()));

    qtb->addSeparator();
    auto normalViewButton = new QPushButton("XYZ", qtb);
    normalViewButton->setFixedSize(50, 30);
    qtb->addWidget(normalViewButton);
    auto yOzViewButton = new QPushButton("yOz", qtb);
    yOzViewButton->setFixedSize(50, 30);
    qtb->addWidget(yOzViewButton);
    auto xOyViewButton = new QPushButton("xOy", qtb);
    xOyViewButton->setFixedSize(50, 30);
    qtb->addWidget(xOyViewButton);
    auto zOxViewButton = new QPushButton("zOx", qtb);
    zOxViewButton->setFixedSize(50, 30);
    qtb->addWidget(zOxViewButton);

    connect(normalViewButton, SIGNAL(clicked()), this, SLOT(slotToNormal()));
    connect(yOzViewButton, SIGNAL(clicked()), this, SLOT(slotToYoZ()));
    connect(xOyViewButton, SIGNAL(clicked()), this, SLOT(slotToXoY()));
    connect(zOxViewButton, SIGNAL(clicked()), this, SLOT(slotToZoX()));

    this->addToolBar(Qt::TopToolBarArea, qtb);
}

void MainWindow::createOpQueWidget() {
    opQueWid = new QListWidget(this);
    opQueWid->setFlow(QListView::LeftToRight);
    opQueWid->setSpacing(6);
    connect(opQueWid, SIGNAL(itemClicked(QListWidgetItem *)),
            SLOT(slotQueItemClicked(QListWidgetItem *)));
    auto delAct = new QAction("Delete", opQueWid);
    opQueWid->addAction(delAct);
    connect(delAct, SIGNAL(triggered()), SLOT(slotOpItemDelete()));
    opQueWid->setContextMenuPolicy(Qt::ActionsContextMenu);
    opQueWid->setFixedHeight(39);

    appQueBut = new QPushButton("Apply queue", opQueWid);
    appQueBut->setFixedSize(70, 40);
    connect(appQueBut, SIGNAL(clicked()), SLOT(slotApplyQue()));
    clrQueBut = new QPushButton("Clear queue", opQueWid);
    clrQueBut->setFixedSize(70, 40);
    connect(clrQueBut, SIGNAL(clicked()), opQueWid, SLOT(clear()));

    auto qtb = new QToolBar("Operators queue", this);
    qtb->addWidget(opQueWid);
    qtb->addWidget(appQueBut);
    qtb->addWidget(clrQueBut);
    qtb->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
    qtb->setFixedHeight(41);
    this->addToolBar(Qt::BottomToolBarArea, qtb);
}

void MainWindow::createSideWidget() {
    auto leftWid = new QWidget(this);
    auto mainLay = new QVBoxLayout(leftWid);

    sphereLabel = new QLabel("Sphere:", leftWid);
    spherePlusBut = new QPushButton("+", leftWid);
    spherePlusBut->setFixedSize(50, 30);
    sphereMinusBut = new QPushButton("-", leftWid);
    sphereMinusBut->setFixedSize(50, 30);
    sphereMinusBut->setEnabled(false);

    connect(spherePlusBut, SIGNAL(clicked()), SLOT(slotPlusSphere()));
    connect(sphereMinusBut, SIGNAL(clicked()), SLOT(slotMinusSphere()));

    auto vectorSphereCreatorWid = new QWidget(leftWid);
    auto vectorSphereLay = new QHBoxLayout(vectorSphereCreatorWid);
    vectorSphereLay->setMargin(0);
    vectorSphereLay->addWidget(sphereLabel);
    vectorSphereLay->addWidget(spherePlusBut);
    vectorSphereLay->addWidget(sphereMinusBut);
    vectorSphereCreatorWid->setLayout(vectorSphereLay);

    topTabWid = new QTabWidget(leftWid);
    topTabWid->setFixedHeight(115);

    auto vct = new Vector(0., 0.);
    addVector(vct, vectors, spheres[0]);
    vct->setName(QString::number(vectors.size()));
    vct->setColorByNameIndex();
    auto vectorWidget = new VectorWidget(topTabWid, vct);
    connect(vectorWidget, SIGNAL(signalUpdate()), this, SLOT(slotUpdateSpheres()));
    circuit->addQubit(vct);

    topTabWid->addTab(vectorWidget, "1");
    mainLay->addWidget(vectorSphereCreatorWid);
    mainLay->addWidget(topTabWid);
    mainLay->addWidget(makeDecompWid());
    mainLay->addWidget(makeOpWid());
    mainLay->setContentsMargins(5, 11, 20, 11);
    mainLay->setSpacing(0);
    leftWid->setLayout(mainLay);
    leftWid->setFixedWidth(400);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidget(leftWid);
    scrollArea->horizontalScrollBar()->setHidden(true);

    auto qtb = new QToolBar("Control panel", this);
    qtb->addWidget(scrollArea);
    qtb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);

    this->addToolBar(Qt::LeftToolBarArea, qtb);
}

QWidget *MainWindow::makeDecompWid() {
    rxyzTab = new QTabWidget(this);
    rxyzTab->insertTab(0, makeZyWid(), "Z-Y");
    rxyzTab->insertTab(1, makeZxWid(), "Z-X");
    rxyzTab->insertTab(2, makeXyWid(), "X-Y");
    rxyzTab->insertTab(3, makeZyxWid(), "Z-Y-X");
    rxyzTab->setCurrentIndex(0);

    auto rtW = new QWidget(this);
    auto qwb = new QVBoxLayout(rtW);
    qwb->addWidget(rxyzTab);
    qwb->setSpacing(0);
    qwb->setMargin(1);
    rtW->setLayout(qwb);
    rtW->setFixedHeight(170);

    return rtW;
}

QWidget *MainWindow::makeZyWid() {
    auto rzW = new QWidget(this);

    rZyAlpEd = new QLineEdit("0", rzW);
    rZyBetEd = new QLineEdit("0", rzW);
    rZyGamEd = new QLineEdit("0", rzW);
    rZyDelEd = new QLineEdit("0", rzW);

    auto rZYALab = new QLabel("Alpha", rzW);
    auto rZYBLab = new QLabel("Rz(Beta)", rzW);
    auto rZYGLab = new QLabel("Ry(Gamma)", rzW);
    auto rZYDLab = new QLabel("Rz(Delta)", rzW);
    auto bRotXYZ = new QPushButton("Set", rzW);

    bRotXYZ->setMaximumWidth(60);
    rZyAlpEd->setMaximumWidth(60);
    rZyBetEd->setMaximumWidth(60);
    rZyGamEd->setMaximumWidth(60);
    rZyDelEd->setMaximumWidth(60);

    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    auto rzLay = new QGridLayout(rzW);
    rzLay->addWidget(rZYALab, 1, 2);
    rzLay->addWidget(rZyAlpEd, 1, 3);
    rzLay->addWidget(rZYBLab, 2, 2);
    rzLay->addWidget(rZyBetEd, 2, 3);
    rzLay->addWidget(rZYGLab, 3, 2);
    rzLay->addWidget(rZyGamEd, 3, 3);
    rzLay->addWidget(rZYDLab, 4, 2);
    rzLay->addWidget(rZyDelEd, 4, 3);
    rzLay->addWidget(bRotXYZ, 5, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeZxWid() {
    auto rzW = new QWidget(this);

    rZxAlpEd = new QLineEdit("0", rzW);
    rZxBetEd = new QLineEdit("0", rzW);
    rZxGamEd = new QLineEdit("0", rzW);
    rZxDelEd = new QLineEdit("0", rzW);

    auto rZXALab = new QLabel("Alpha", rzW);
    auto rZXBLab = new QLabel("Rz(Beta)", rzW);
    auto rZXGLab = new QLabel("Rx(Gamma)", rzW);
    auto rZXDLab = new QLabel("Rz(Delta)", rzW);
    auto bRotXYZ = new QPushButton("Set", rzW);

    bRotXYZ->setMaximumWidth(60);
    rZxAlpEd->setMaximumWidth(60);
    rZxBetEd->setMaximumWidth(60);
    rZxGamEd->setMaximumWidth(60);
    rZxDelEd->setMaximumWidth(60);

    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    auto rzLay = new QGridLayout(rzW);
    rzLay->addWidget(rZXALab, 1, 2);
    rzLay->addWidget(rZxAlpEd, 1, 3);
    rzLay->addWidget(rZXBLab, 2, 2);
    rzLay->addWidget(rZxBetEd, 2, 3);
    rzLay->addWidget(rZXGLab, 3, 2);
    rzLay->addWidget(rZxGamEd, 3, 3);
    rzLay->addWidget(rZXDLab, 4, 2);
    rzLay->addWidget(rZxDelEd, 4, 3);
    rzLay->addWidget(bRotXYZ, 5, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeXyWid() {
    auto rzW = new QWidget(this);

    rXyAlpEd = new QLineEdit("0", rzW);
    rXyBetEd = new QLineEdit("0", rzW);
    rXyGamEd = new QLineEdit("0", rzW);
    rXyDelEd = new QLineEdit("0", rzW);

    auto rXYALab = new QLabel("Alpha", rzW);
    auto rXYBLab = new QLabel("Rx(Beta)", rzW);
    auto rXYGLab = new QLabel("Ry(Gamma)", rzW);
    auto rXYDLab = new QLabel("Rx(Delta)", rzW);
    auto bRotXYZ = new QPushButton("Set", rzW);

    bRotXYZ->setMaximumWidth(60);
    rXyAlpEd->setMaximumWidth(60);
    rXyBetEd->setMaximumWidth(60);
    rXyGamEd->setMaximumWidth(60);
    rXyDelEd->setMaximumWidth(60);

    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    auto rzLay = new QGridLayout(rzW);
    rzLay->addWidget(rXYALab, 1, 2);
    rzLay->addWidget(rXyAlpEd, 1, 3);
    rzLay->addWidget(rXYBLab, 2, 2);
    rzLay->addWidget(rXyBetEd, 2, 3);
    rzLay->addWidget(rXYGLab, 3, 2);
    rzLay->addWidget(rXyGamEd, 3, 3);
    rzLay->addWidget(rXYDLab, 4, 2);
    rzLay->addWidget(rXyDelEd, 4, 3);
    rzLay->addWidget(bRotXYZ, 5, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeZyxWid() {
    auto rzW = new QWidget(this);

    rZyxAlpEd = new QLineEdit("0", rzW);
    rZyxBetEd = new QLineEdit("0", rzW);
    rZyxGamEd = new QLineEdit("0", rzW);
    rZyxDelEd = new QLineEdit("0", rzW);

    auto rZyxALab = new QLabel("Alpha", rzW);
    auto rZyxBLab = new QLabel("Rz(Beta)", rzW);
    auto rZyxGLab = new QLabel("Ry(Gamma)", rzW);
    auto rZyxDLab = new QLabel("Rx(Delta)", rzW);
    auto bRotXYZ = new QPushButton("Set", rzW);

    bRotXYZ->setMaximumWidth(60);
    rZyxAlpEd->setMaximumWidth(60);
    rZyxBetEd->setMaximumWidth(60);
    rZyxGamEd->setMaximumWidth(60);
    rZyxDelEd->setMaximumWidth(60);

    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    auto rzLay = new QGridLayout(rzW);
    rzLay->addWidget(rZyxALab, 1, 2);
    rzLay->addWidget(rZyxAlpEd, 1, 3);
    rzLay->addWidget(rZyxBLab, 2, 2);
    rzLay->addWidget(rZyxBetEd, 2, 3);
    rzLay->addWidget(rZyxGLab, 3, 2);
    rzLay->addWidget(rZyxGamEd, 3, 3);
    rzLay->addWidget(rZyxDLab, 4, 2);
    rzLay->addWidget(rZyxDelEd, 4, 3);
    rzLay->addWidget(bRotXYZ, 5, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeOpWid() {
    auto opW = new QWidget(this);

    stackW = new QTabWidget(opW);
    stackW->setFixedHeight(120);

    auto bOp = new QWidget(stackW);
    auto bOpLay = new QGridLayout(bOp);
    bOpLay->addWidget(makeOpButton("X"), 0, 0, 1, 4);
    bOpLay->addWidget(makeOpButton("Y"), 0, 4, 1, 4);
    bOpLay->addWidget(makeOpButton("Z"), 0, 8, 1, 4);
    bOpLay->addWidget(makeOpButton("H"), 1, 0, 1, 4);
    bOpLay->addWidget(makeOpButton("S"), 1, 4, 1, 4);
    bOpLay->addWidget(makeOpButton("T"), 1, 8, 1, 4);
    bOpLay->addWidget(makeOpButton("Phi"), 2, 0, 1, 3);
    bOpLay->addWidget(makeOpButton("Rx"), 2, 3, 1, 3);
    bOpLay->addWidget(makeOpButton("Ry"), 2, 6, 1, 3);
    bOpLay->addWidget(makeOpButton("Rz"), 2, 9, 1, 3);
    bOpLay->setContentsMargins(10, 5, 10, 5);
    bOpLay->setSpacing(2);
    bOp->setLayout(bOpLay);

    stackW->addTab(bOp, "Gates");

    auto mOp = new QWidget(opW);
    auto mOpLay = new QGridLayout(mOp);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            mat[i][j] = new QLineEdit(QString::number(i == j), mOp);
            mat[i][j]->setValidator(Utility::compValid());
            connect(mat[i][j], SIGNAL(textEdited(QString)),
                    SLOT(slotComplexLineEditChanged(QString)));
        }
    }

    auto applyMat = new QPushButton("Ok", mOp);
    connect(applyMat, SIGNAL(clicked()), SLOT(slotSetMatrixOp()));

    auto getRandomBut = new QPushButton("Random", mOp);
    connect(getRandomBut, SIGNAL(clicked()), SLOT(slotSetRandomOp()));

    mOpLay->addWidget(mat[0][0], 1, 0);
    mOpLay->addWidget(mat[0][1], 1, 5);
    mOpLay->addWidget(mat[1][0], 2, 0);
    mOpLay->addWidget(mat[1][1], 2, 5);
    mOpLay->addWidget(applyMat, 3, 5);
    mOpLay->addWidget(getRandomBut, 3, 0);
    mOpLay->setContentsMargins(10, 10, 10, 0);
    mOp->setLayout(mOpLay);

    stackW->addTab(mOp, "Matrix");

    auto rNw = new QWidget(opW);
    axRnEd = new QLineEdit("0;0;0", rNw);
    axRnEd->setValidator(Utility::axisValid());
    ngRnEd = new QLineEdit("0", rNw);
    ngRnEd->setValidator(new QDoubleValidator(ngRnEd));

    auto axOpBut = new QPushButton("Ok", rNw);
    connect(axOpBut, SIGNAL(clicked()), SLOT(slotSetAxOp()));

    auto rNwLay = new QGridLayout(opW);
    rNwLay->addWidget(new QLabel("Vector", rNwLay->widget()), 0, 0);
    rNwLay->addWidget(axRnEd, 0, 1, 1, 3);
    rNwLay->addWidget(new QLabel("Angle", rNwLay->widget()), 1, 0);
    rNwLay->addWidget(ngRnEd, 1, 1, 1, 3);
    rNwLay->addWidget(axOpBut, 2, 3, 1, 1); // 4
    rNwLay->setContentsMargins(20, 10, 20, 0);
    rNwLay->setSpacing(5);
    rNw->setLayout(rNwLay);

    stackW->addTab(rNw, "Rn");

    auto qGb = new QGroupBox("Rotation", opW);

    rZyRb = new QRadioButton("ZY-decomposition", qGb);
    rZyRb->toggle();

    rZxRb = new QRadioButton("ZX-decomposition", qGb);
    rXyRb = new QRadioButton("XY-decomposition", qGb);
    rZyxRb = new QRadioButton("ZYX-decomposition", qGb);
    rtRb = new QRadioButton("Rotation about vector", qGb);

    connect(rtRb, SIGNAL(toggled(bool)), SLOT(slotToggleRotateVector(bool)));

    auto gbLay = new QVBoxLayout(qGb);
    qGb->setFixedHeight(120);
    gbLay->addWidget(rZyRb);
    gbLay->addWidget(rZxRb);
    gbLay->addWidget(rXyRb);
    gbLay->addWidget(rZyxRb);
    gbLay->addWidget(rtRb);
    gbLay->setSpacing(5);
    qGb->setLayout(gbLay);

    appBut = new QPushButton("Apply Id", opW);
    appBut->setToolTip(curOperator.getCurOperatorMatrixStr());
    appBut->setFixedHeight(35);
    connect(appBut, SIGNAL(clicked()), SLOT(slotApplyOp()));

    auto addToQueBut = new QPushButton("Add to queue", opW);
    addToQueBut->setFixedHeight(35);
    connect(addToQueBut, SIGNAL(clicked()), SLOT(slotAddToQue()));

    auto pLay = new QVBoxLayout(opW);
    auto horLay = new QHBoxLayout(pLay->widget());
    horLay->addWidget(appBut);
    horLay->addWidget(addToQueBut);

    pLay->addWidget(stackW);
    pLay->addWidget(qGb);
    pLay->addStretch();
    pLay->addLayout(horLay);

    pLay->setMargin(2);
    pLay->setSpacing(0);
    opW->setLayout(pLay);

    return opW;
}

QPushButton *MainWindow::makeOpButton(const QString &str) {
    auto newOpBut = new QPushButton(str, this);
    newOpBut->setFixedHeight(26);
    connect(newOpBut, SIGNAL(clicked()), SLOT(slotSetOperatorClicked()));
    return newOpBut;
}

void MainWindow::slotShowTrace() {
    foreach (auto &e, vectors.keys()) { e->setEnableTrace(showTAct->isChecked()); }
    slotUpdateSpheres();
}

void MainWindow::slotClearTrace() {
    foreach (auto &e, vectors.keys()) { e->clearTrace(); }
    slotUpdateSpheres();
}

void MainWindow::slotTraceColor(int index) {
    QColor clr;
    switch (index) {
    case 0:
        clr = QColor(Qt::red);
        break;
    case 1:
        clr = QColor(Qt::green);
        break;
    case 2:
        clr = QColor(Qt::blue);
        break;
    case 3:
        clr = QColor(Qt::gray);
        break;
    case 4:
        clr = QColor(Qt::yellow);
        break;
    default:
        clr = QColor(Qt::black);
    }
    foreach (auto &e, vectors.keys()) { e->setTraceColor(clr); }
}

void MainWindow::slotSetOperatorClicked() {
    std::string  str = ((QPushButton *)sender())->text().toStdString();
    BlochDialog *aIn;

    switch (str[0]) {
    case 'X':
        curOperator.toX();
        break;
    case 'Y':
        curOperator.toY();
        break;
    case 'Z':
        curOperator.toZ();
        break;
    case 'H':
        curOperator.toH();
        break;
    case 'S':
        curOperator.toS();
        break;
    case 'T':
        curOperator.toT();
        break;
    case 'P':
        aIn = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            curOperator.toPhi(aIn->ang().toDouble() * M_PI / 180);
        } else {
            delete aIn;
            return;
        }
        delete aIn;
        break;
    case 'R':
        aIn = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::ANGLE);
        if (aIn->exec() == QDialog::Accepted) {
            double the = (aIn->ang().toDouble()) * M_PI / 180;
            if (str[1] == 'x')
                curOperator.toXrotate(the);
            else if (str[1] == 'y')
                curOperator.toYrotate(the);
            else if (str[1] == 'z')
                curOperator.toZrotate(the);
        } else {
            delete aIn;
            return;
        }
        delete aIn;
        break;
    }
    updateOp();
}

void MainWindow::slotSetRXYZOp() {
    decomposition dec;
    switch (rxyzTab->currentIndex()) {
    case 0:
        dec.alpha = rZyAlpEd->text().toDouble() / 180 * M_PI;
        dec.beta = rZyBetEd->text().toDouble() / 180 * M_PI;
        dec.gamma = rZyGamEd->text().toDouble() / 180 * M_PI;
        dec.delta = rZyDelEd->text().toDouble() / 180 * M_PI;
        assert(curOperator.setOperatorByZyDecomposition(dec));
        break;
    case 1:
        dec.alpha = rZxAlpEd->text().toDouble() / 180 * M_PI;
        dec.beta = rZxBetEd->text().toDouble() / 180 * M_PI;
        dec.gamma = rZxGamEd->text().toDouble() / 180 * M_PI;
        dec.delta = rZxDelEd->text().toDouble() / 180 * M_PI;
        assert(curOperator.setOperatorByZxDecomposition(dec));
        break;
    case 2:
        dec.alpha = rXyAlpEd->text().toDouble() / 180 * M_PI;
        dec.beta = rXyBetEd->text().toDouble() / 180 * M_PI;
        dec.gamma = rXyGamEd->text().toDouble() / 180 * M_PI;
        dec.delta = rXyDelEd->text().toDouble() / 180 * M_PI;
        assert(curOperator.setOperatorByXyDecomposition(dec));
        break;
    case 3:
        dec.alpha = rZyxAlpEd->text().toDouble() / 180 * M_PI;
        dec.beta = rZyxBetEd->text().toDouble() / 180 * M_PI;
        dec.gamma = rZyxGamEd->text().toDouble() / 180 * M_PI;
        dec.delta = rZyxDelEd->text().toDouble() / 180 * M_PI;
        assert(curOperator.setOperatorByZyxDecomposition(dec));
        break;
    }

    updateOp();
}

void MainWindow::slotSetMatrixOp() {
    QVector<complex> res;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            try {
                res.push_back(Utility::parseStrToComplex(mat[i][j]->text()));
            } catch (int e) {
                QMessageBox::warning(this, "Error",
                                     QString("Wrong input: Matrix(%1,%2)").arg(i).arg(j));
                return;
            }
        }
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix({res[0], res[1], res[2], res[3]})) {
        QMessageBox::warning(this, "Error", "Matrix must be unitary");
        return;
    }
    curOperator.setOperator(matrixOp, Operator::getOperatorName(matrixOp));

    updateOp(OPERATOR_FORM::MATRIX);
}

void MainWindow::slotSetRandomOp() {
    curOperator.toRandUnitaryMatrix();

    updateOp();
}

void MainWindow::slotSetAxOp() {
    QRegExp rxp("^(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?"
                "[\\d]*e?[+-]?[\\d]*)$");
    if (axRnEd->text().contains(rxp)) {
        vectorangle va;
        va.angle = ngRnEd->text().toDouble() * M_PI / 180;
        va.x = rxp.capturedTexts()[1].toDouble();
        va.y = rxp.capturedTexts()[2].toDouble();
        va.z = rxp.capturedTexts()[3].toDouble();
        if (std::abs(va.x) + std::abs(va.y) + std::abs(va.z) < EPSILON) {
            QMessageBox::warning(this, "Warning", "Vector must be non-zero");
            return;
        }

        double len = sqrt(va.x * va.x + va.y * va.y + va.z * va.z);
        if (not Utility::fuzzyCompare(len, 1.)) {
            auto dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
            if (isAutoNormalize or dial->exec() == QDialog::Accepted) {
                va.x /= len;
                va.y /= len;
                va.z /= len;
                axRnEd->setText(QString("%1;%2;%3")
                                    .arg(Utility::roundNumber(va.x))
                                    .arg(Utility::roundNumber(va.y))
                                    .arg(Utility::roundNumber(va.z)));
            } else {
                return;
            }
        }

        assert(curOperator.setOperatorByVectorAngle(va));

        updateOp(OPERATOR_FORM::VECTOR);
    } else {
        QMessageBox::warning(this, "Error", "Wrong input. Must be x;y;z");
        return;
    }
}

void MainWindow::updateOp(OPERATOR_FORM exclude) {
    appBut->setText("Apply " + curOperator.getOperatorName());

    decomposition zyDec = curOperator.zyDecomposition();
    rZyAlpEd->setText(Utility::numberToStr(zyDec.alpha * 180 / M_PI));
    rZyBetEd->setText(Utility::numberToStr(zyDec.beta * 180 / M_PI));
    rZyGamEd->setText(Utility::numberToStr(zyDec.gamma * 180 / M_PI));
    rZyDelEd->setText(Utility::numberToStr(zyDec.delta * 180 / M_PI));

    decomposition zxDec = curOperator.zxDecomposition();
    rZxAlpEd->setText(Utility::numberToStr(zxDec.alpha * 180 / M_PI));
    rZxBetEd->setText(Utility::numberToStr(zxDec.beta * 180 / M_PI));
    rZxGamEd->setText(Utility::numberToStr(zxDec.gamma * 180 / M_PI));
    rZxDelEd->setText(Utility::numberToStr(zxDec.delta * 180 / M_PI));

    decomposition xyDec = curOperator.xyDecomposition();
    rXyAlpEd->setText(Utility::numberToStr(xyDec.alpha * 180 / M_PI));
    rXyBetEd->setText(Utility::numberToStr(xyDec.beta * 180 / M_PI));
    rXyGamEd->setText(Utility::numberToStr(xyDec.gamma * 180 / M_PI));
    rXyDelEd->setText(Utility::numberToStr(xyDec.delta * 180 / M_PI));

    decomposition zyxDec = curOperator.zyxDecomposition();
    rZyxAlpEd->setText(Utility::numberToStr(zyxDec.alpha * 180 / M_PI));
    rZyxBetEd->setText(Utility::numberToStr(zyxDec.beta * 180 / M_PI));
    rZyxGamEd->setText(Utility::numberToStr(zyxDec.gamma * 180 / M_PI));
    rZyxDelEd->setText(Utility::numberToStr(zyxDec.delta * 180 / M_PI));

    if (exclude != OPERATOR_FORM::MATRIX) {
        mat[0][0]->setText(Utility::parseComplexToStr(curOperator.getOperator().a()));
        mat[0][1]->setText(Utility::parseComplexToStr(curOperator.getOperator().b()));
        mat[1][0]->setText(Utility::parseComplexToStr(curOperator.getOperator().c()));
        mat[1][1]->setText(Utility::parseComplexToStr(curOperator.getOperator().d()));
        appBut->setToolTip(curOperator.getCurOperatorMatrixStr());
    }

    vectorangle va = curOperator.vectorAngleDec();
    foreach (auto e, vectors.keys()) { e->setRotateVector(QVector3D(va.x, va.y, va.z)); }

    if (exclude != OPERATOR_FORM::VECTOR) {
        axRnEd->setText(QString("%1;%2;%3")
                            .arg(Utility::roundNumber(va.x))
                            .arg(Utility::roundNumber(va.y))
                            .arg(Utility::roundNumber(va.z)));
        ngRnEd->setText(QString("%1").arg(Utility::roundNumber(va.angle * 180 / M_PI)));
    }

    slotUpdateSpheres();
}

void MainWindow::slotQueItemClicked(QListWidgetItem *it) {
    auto opi = (OpItem *)it;
    curOperator = opi->getOp();
    updateOp();
}

void MainWindow::slotOpItemDelete() {
    if (!opQueWid->selectedItems().isEmpty())
        delete opQueWid->currentItem();
}

void MainWindow::slotAddToQue() {
    auto it = new OpItem(opQueWid, curOperator.getOperatorName(), curOperator);
    opQueWid->insertItem(0, it);
}

void MainWindow::slotApplyOp() {
    stopTimer();
    foreach (auto e, vectors.keys()) { startMove(e, getCurrentDecomposition()); }
}

void MainWindow::slotApplyQue() {
    stopTimer();
    singleOperator = curOperator;
    for (int i = 0; i < opQueWid->count(); ++i) {
        opQueue.append((OpItem *)(opQueWid->item(i)));
    }

    if (not opQueue.isEmpty()) {
        curOperator = opQueue.last()->getOp();
        opQueue.last()->setBackground(QBrush(Qt::red));
        updateOp();
        slotApplyOp();
        isQueueAnimation = true;
    }
}

void MainWindow::startMove(Vector *v, CurDecompFun getDec) {
    v->changeVector((curOperator.*getDec)(v->getSpike()));
    v->setOperator(curOperator.getOperatorName());
    v->setAnimateState(true);
    startTimer();
}

void MainWindow::startMove(Vector *v, Operator &op, CurDecompFun getDec) {
    vectorangle va = op.vectorAngleDec();
    v->setRotateVector(QVector3D(va.x, va.y, va.z));
    v->setOperator(op.getOperatorName());
    v->changeVector((op.*getDec)(v->getSpike()));
    v->setAnimateState(true);
    startTimer();
}

void MainWindow::nextAnimStepCircuit() {
    stopTimer();
    isCircuitAnimation = true;

    foreach (auto e, circuit->getQubits()) {
        startMove(e->getVector(), e->getOperator(circuit->getCurrentStep()),
                  getCurrentDecomposition());
    }
}

CurDecompFun MainWindow::getCurrentDecomposition() {
    if (rtRb->isChecked()) {
        return &Operator::applyVectorRotation;
    } else if (rZyRb->isChecked()) {
        return &Operator::applyZyDecomposition;
    } else if (rZxRb->isChecked()) {
        return &Operator::applyZxDecomposition;
    } else if (rXyRb->isChecked()) {
        return &Operator::applyXyDecomposition;
    } else if (rZyxRb->isChecked()) {
        return &Operator::applyZyxDecomposition;
    }
    return &Operator::applyZyDecomposition;
}

void MainWindow::slotAbout() {
    QString aboutText =
        "<h3 class=\"code-line\" data-line-start=0 data-line-end=1 ><a "
        "id=\"blochsphere_0\"></a>blochsphere " BIT_VERSION " " BLOCHSPHERE_VERSION "</h3>\n"
        "<hr>\n"
        "<p class=\"has-line-data\" data-line-start=\"2\" data-line-end=\"3\">"
        "A Bloch sphere emulator program.</p>\n"
        "<p class=\"has-line-data\" data-line-start=\"4\" data-line-end=\"5\">Copyright ?? 2022 "
        "Vasiliy Stephanov <a href=\"mailto:baseoleph@gmail.com\">baseoleph@gmail.com</a></p>\n"
        "Source code: <a href=https://github.com/baseoleph/blochsphere>"
        "https://github.com/baseoleph/blochsphere</a>\n"
        "<hr>\n"
        "<p class=\"has-line-data\" data-line-start=\"6\" data-line-end=\"7\">Based on <a "
        "href=\"https://github.com/baseoleph/blochsphere/tree/base\">BLOCH</a> by Denis "
        "Kadochnikov (?????????? ????????????????????)</p>\n"
        "<p class=\"has-line-data\" data-line-start=\"8\" data-line-end=\"9\">Decomposition "
        "algorithms by M.V. Shvetskiy (???????????? ???????????????????????? ??????????????) <a "
        "href=\"mailto:audit269@gmail.com\">audit269@gmail.com</a>.</p>\n";

    QMessageBox::about(this, "blochsphere", aboutText);
    if (++easterEggCounter > 2) {
        easterEggCounter = 0;
        spheres.first()->easterEggRotate();
    }
}

void MainWindow::slotReset() {
    stopTimer();
    controlWidget->hide();
    while (not spheres.empty()) {
        foreach (auto e, vectors.keys()) {
            if (vectors[e].indexOf(spheres.last()) != -1) {
                removeVector(e, vectors);
            }
        }
        spheres.last()->~Sphere();
        spheres.pop_back();

        delete topTabWid->widget(spheres.size());
        topTabWid->removeTab(spheres.size());
        circuit->removeQubit();
    }

    while (circuit->getSizeOfSteps() != 1) {
        circuit->slotRemoveStep();
    }

    controlWidget->show();
    slotPlusSphere();
}

void MainWindow::slotClear() {
    stopTimer();

    Vector v(0., 0.);
    foreach (auto e, topTabWid->findChildren<VectorWidget *>()) {
        if (e->getVector() != nullptr) {
            e->getVector()->changeVector(v.getSpike());
            e->fillFieldsOfVector(e->getVector()->getSpike());
            e->getVector()->clearTrace();
        }
    }

    foreach (auto e, spheres) { e->toNormal(); }
}

void MainWindow::slotComplexLineEditChanged(const QString &) {
    Utility::updateComplexLineEdit(mat[0][0]);
    Utility::updateComplexLineEdit(mat[0][1]);
    Utility::updateComplexLineEdit(mat[1][0]);
    Utility::updateComplexLineEdit(mat[1][1]);
}

void MainWindow::slotToggleRotateVector(bool f) {
    foreach (auto e, vectors.keys()) { e->setEnabledRotateVector(f); }
    slotUpdateSpheres();
}

void MainWindow::slotToggleAutoNormalize(bool f) {
    isAutoNormalize = f;
    foreach (auto e, topTabWid->findChildren<VectorWidget *>()) {
        e->setAutoNormalise(isAutoNormalize);
    }
}

void MainWindow::slotPlusSphere() {
    if (spheres.size() < MAX_COUNT_SPHERES) {
        spheres.append(new Sphere(controlWidget));
        sphereLayout->addWidget(spheres.last());

        auto vct = new Vector(0., 0.);
        addVector(vct, vectors, spheres.last());
        vct->setName(QString::number(vectors.size()));
        vct->setColorByNameIndex();

        auto vectorWidget = new VectorWidget(topTabWid, vct);
        connect(vectorWidget, SIGNAL(signalUpdate()), this, SLOT(slotUpdateSpheres()));
        topTabWid->addTab(vectorWidget, QString::number(spheres.size()));
        circuit->addQubit(vct);

        vectorangle va = curOperator.vectorAngleDec();
        vct->setRotateVector(QVector3D(va.x, va.y, va.z));
        vct->setEnabledRotateVector(rtRb->isChecked());
    }

    spherePlusBut->setEnabled(spheres.size() < MAX_COUNT_SPHERES);
    sphereMinusBut->setEnabled(spheres.size() > 1);
}

void MainWindow::slotMinusSphere() {
    if (not spheres.empty()) {
        foreach (auto e, vectors.keys()) {
            if (vectors[e].indexOf(spheres.last()) != -1) {
                removeVector(e, vectors);
            }
        }
        spheres.last()->~Sphere();
        spheres.pop_back();

        delete topTabWid->widget(spheres.size());
        topTabWid->removeTab(spheres.size());
        circuit->removeQubit();
    }

    spherePlusBut->setEnabled(spheres.size() < MAX_COUNT_SPHERES);
    sphereMinusBut->setEnabled(spheres.size() > 1);
}

void MainWindow::startTimer() {
    if (not tm->isActive()) {
        tm->start(10);
        emit signalAnimating(true);
    }
    setEnabledWidgets(false);
}

void MainWindow::stopTimer() {
    tm->stop();
    isCircuitAnimation = false;
    isQueueAnimation = false;
    foreach (auto e, vectors.keys()) { e->setOperator(""); }
    emit signalAnimating(false);
    setEnabledWidgets(true);
    slotUpdateSpheres();
}

void MainWindow::setEnabledWidgets(bool f) {
    appBut->setEnabled(f);
    appQueBut->setEnabled(f);
    spherePlusBut->setEnabled(f and spheres.size() < MAX_COUNT_SPHERES);
    sphereMinusBut->setEnabled(f and spheres.size() > 1);
    circuit->runCircuitBut->setEnabled(f);
    circuit->addStepBut->setEnabled(f and circuit->getSizeOfSteps() < MAX_COUNT_OF_STEPS);
    circuit->removeStepBut->setEnabled(f and circuit->getSizeOfSteps() > 1);
    clrQueBut->setEnabled(f);
}

void MainWindow::slotStartCircuitMove() {
    circuit->clearStepPos();
    nextAnimStepCircuit();
}

void MainWindow::slotUpdateSpheres() {
    foreach (auto e, spheres) { e->update(); }
}

void MainWindow::slotSpeedUp() {
    if (Utility::getSpeed() < 10) {
        Utility::setSpeed(Utility::getSpeed() + 1);
    }
    speedLabel->setText("Speed: " + QString::number(Utility::getSpeed()) + " ");
    speedUpButton->setEnabled(Utility::getSpeed() < 10);
    speedDownButton->setEnabled(Utility::getSpeed() > 1);
}

void MainWindow::slotSpeedDown() {
    if (Utility::getSpeed() > 0) {
        Utility::setSpeed(Utility::getSpeed() - 1);
    }
    speedLabel->setText("Speed: " + QString::number(Utility::getSpeed()) + " ");
    speedUpButton->setEnabled(Utility::getSpeed() < 10);
    speedDownButton->setEnabled(Utility::getSpeed() > 1);
}

void MainWindow::slotToNormal() {
    foreach (auto e, spheres) { e->toNormal(); }
}

void MainWindow::slotToYoZ() {

    foreach (auto e, spheres) { e->toYoZ(); }
}

void MainWindow::slotToXoY() {

    foreach (auto e, spheres) { e->toXoY(); }
}

void MainWindow::slotToZoX() {

    foreach (auto e, spheres) { e->toZoX(); }
}
