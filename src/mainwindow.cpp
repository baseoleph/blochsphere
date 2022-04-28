// The program helps to see a geometric representation of qubits
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

#include "mainwindow.hpp"
#include "blochdialog.hpp"
#include "operator.hpp"
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

// TODO move to another place
QRegExpValidator
    compValid(QRegExp(QString::fromUtf8("^[+-]?[0-9]*\\.?[0-9]*[+-]?[0-9]*\\.?[0-9]*[iIшШ]?$")));
QRegExpValidator axisValid(QRegExp("^-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*$"));

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createSphere();
    createActions();
    createMenu();
    createStatusBar();
    createTopBar();
    createSideWidget();
    createOpQueWidget();

    tm = new QTimer(this);
    connect(tm, SIGNAL(timeout()), SLOT(slotTimer()));
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
            } else {
                stopTimer();
                curOperator = singleOperator;
                isQueueAnimation = false;
            }
        } else if (isCircuitAnimation) {
            circuitStepNumber += 1;
            if (circuitStepNumber >= circuit->getSizeOfSteps()) {
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

    //    saveState = new QAction("Save state", this);
    //    connect(saveState, SIGNAL(triggered()), SLOT(slotSaveState()));
    //
    //    recallState = new QAction("Recall state", this);
    //    recallState->setEnabled(false);
    //    connect(recallState, SIGNAL(triggered()), SLOT(slotRecallState()));

    resetAct = new QAction("Reset", this);
    connect(resetAct, SIGNAL(triggered()), SLOT(slotReset()));

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
    auto *mnuBar = new QMenuBar();
    auto *menuFile = new QMenu("File");
    auto *menuInfo = new QMenu("Info");

    menuFile->addSeparator();
    menuFile->addAction(exitAct);
    menuInfo->addAction(aboutAct);

    mnuBar->addMenu(menuFile);
    mnuBar->addMenu(menuInfo);

    this->setMenuBar(mnuBar);
}

void MainWindow::createTopBar() {
    auto *qtb = new QToolBar("Tool bar");
    //    qtb->addAction(saveState);
    //    qtb->addAction(recallState);
    qtb->addAction(resetAct);
    qtb->addSeparator();
    qtb->addAction(showTAct);
    qtb->addAction(clearTAct);

    colorComboBox = new QComboBox(qtb);
    colorComboBox->addItem("Red");
    colorComboBox->addItem("Green");
    colorComboBox->addItem("Blue");
    colorComboBox->addItem("Grey");
    colorComboBox->addItem("Yellow");
    colorComboBox->setCurrentIndex(0);

    connect(colorComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotTraceColor(int)));

    qtb->addWidget(new QLabel("<center>Trace Color:"));
    qtb->addWidget(colorComboBox);
    qtb->addSeparator();

    auto normalizeCheckBox = new QCheckBox(qtb);
    normalizeCheckBox->setText("Auto normalize vectors");
    normalizeCheckBox->setChecked(true);

    connect(normalizeCheckBox, SIGNAL(toggled(bool)), SLOT(slotToggleAutoNormalize(bool)));

    qtb->addWidget(normalizeCheckBox);
    qtb->addSeparator();

    speedLabel = new QLabel("Speed: 5 ");
    qtb->addWidget(speedLabel);
    speedUpButton = new QPushButton("Up");
    speedUpButton->setFixedSize(50, 30);
    speedDownButton = new QPushButton("Down");
    speedDownButton->setFixedSize(50, 30);
    qtb->addWidget(speedUpButton);
    qtb->addWidget(speedDownButton);

    connect(speedUpButton, SIGNAL(clicked()), this, SLOT(slotSpeedUp()));
    connect(speedDownButton, SIGNAL(clicked()), this, SLOT(slotSpeedDown()));

    this->addToolBar(Qt::TopToolBarArea, qtb);
}

void MainWindow::createOpQueWidget() {
    opQueWid = new QListWidget;
    opQueWid->setFlow(QListView::LeftToRight);
    opQueWid->setSpacing(6);
    connect(opQueWid, SIGNAL(itemClicked(QListWidgetItem *)),
            SLOT(slotQueItemClicked(QListWidgetItem *)));
    auto *delAct = new QAction("Delete", opQueWid);
    opQueWid->addAction(delAct);
    connect(delAct, SIGNAL(triggered()), SLOT(slotOpItemDelete()));
    opQueWid->setContextMenuPolicy(Qt::ActionsContextMenu);
    opQueWid->setFixedHeight(39);

    appQueBut = new QPushButton("Apply queue");
    appQueBut->setFixedSize(70, 40);
    connect(appQueBut, SIGNAL(clicked()), SLOT(slotApplyQue()));
    auto *clrQueBut = new QPushButton("Clear queue");
    clrQueBut->setFixedSize(70, 40);
    connect(clrQueBut, SIGNAL(clicked()), opQueWid, SLOT(clear()));

    auto *qtb = new QToolBar("Operators queue");
    qtb->addWidget(opQueWid);
    qtb->addWidget(appQueBut);
    qtb->addWidget(clrQueBut);
    qtb->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
    qtb->setFixedHeight(41);
    this->addToolBar(Qt::BottomToolBarArea, qtb);
}

void MainWindow::createStatusBar() {
    xyzStLab = new QLabel();
    svdStLab = new QLabel();
    mtnStLab = new QLabel();
    xyzStLab->setFixedWidth(115);
    svdStLab->setFixedWidth(200);
    mtnStLab->setFixedWidth(200);
    svdStLab->hide();
    mtnStLab->hide();
    statusBar()->addWidget(xyzStLab);
    statusBar()->addWidget(svdStLab);
    statusBar()->addWidget(mtnStLab);
}

void MainWindow::createSideWidget() {
    auto leftWid = new QWidget();
    auto mainLay = new QVBoxLayout();

    sphereLabel = new QLabel("Sphere:");
    spherePlusBut = new QPushButton("+");
    spherePlusBut->setFixedWidth(50);
    sphereMinusBut = new QPushButton("-");
    sphereMinusBut->setFixedWidth(50);
    sphereMinusBut->setEnabled(false);

    connect(spherePlusBut, SIGNAL(clicked()), SLOT(slotPlusSphere()));
    connect(sphereMinusBut, SIGNAL(clicked()), SLOT(slotMinusSphere()));

    auto vectorSphereCreatorWid = new QWidget();
    auto vectorSphereLay = new QHBoxLayout();
    vectorSphereLay->setMargin(0);
    vectorSphereLay->addWidget(sphereLabel);
    vectorSphereLay->addWidget(spherePlusBut);
    vectorSphereLay->addWidget(sphereMinusBut);
    vectorSphereCreatorWid->setLayout(vectorSphereLay);

    topTabWid = new QTabWidget();
    topTabWid->setFixedHeight(115);

    // TODO maybe better use function for it
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

    auto qtb = new QToolBar("Control panel");
    qtb->addWidget(scrollArea);
    qtb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);

    this->addToolBar(Qt::LeftToolBarArea, qtb);
}

QWidget *MainWindow::makeDecompWid() {
    rxyzTab = new QTabWidget;
    rxyzTab->insertTab(0, makeZyWid(), "Z-Y");
    rxyzTab->insertTab(1, makeZxWid(), "Z-X");
    rxyzTab->insertTab(2, makeXyWid(), "X-Y");
    rxyzTab->insertTab(3, makeZyxWid(), "Z-Y-X");
    rxyzTab->setCurrentIndex(0);

    auto *bRotXYZ = new QPushButton("Set");
    bRotXYZ->setFixedWidth(60);
    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    auto *qwb = new QVBoxLayout;
    qwb->addWidget(rxyzTab);
    qwb->addWidget(bRotXYZ);
    qwb->setSpacing(0);
    qwb->setMargin(1);
    auto *rtW = new QWidget;
    rtW->setLayout(qwb);
    rtW->setFixedHeight(140);

    return rtW;
}

QWidget *MainWindow::makeZyWid() {
    rZyAlpEd = new QLineEdit("0");
    rZyBetEd = new QLineEdit("0");
    rZyGamEd = new QLineEdit("0");
    rZyDelEd = new QLineEdit("0");

    auto *rZYALab = new QLabel("Alpha");
    auto *rZYBLab = new QLabel("Rz(Beta)");
    auto *rZYGLab = new QLabel("Ry(Gamma)");
    auto *rZYDLab = new QLabel("Rz(Delta)");

    rZyAlpEd->setMaximumWidth(60);
    rZyBetEd->setMaximumWidth(60);
    rZyGamEd->setMaximumWidth(60);
    rZyDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rZYALab, 1, 2);
    rzLay->addWidget(rZyAlpEd, 1, 3);
    rzLay->addWidget(rZYBLab, 2, 2);
    rzLay->addWidget(rZyBetEd, 2, 3);
    rzLay->addWidget(rZYGLab, 3, 2);
    rzLay->addWidget(rZyGamEd, 3, 3);
    rzLay->addWidget(rZYDLab, 4, 2);
    rzLay->addWidget(rZyDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeZxWid() {
    rZxAlpEd = new QLineEdit("0");
    rZxBetEd = new QLineEdit("0");
    rZxGamEd = new QLineEdit("0");
    rZxDelEd = new QLineEdit("0");

    auto *rZXALab = new QLabel("Alpha");
    auto *rZXBLab = new QLabel("Rz(Beta)");
    auto *rZXGLab = new QLabel("Rx(Gamma)");
    auto *rZXDLab = new QLabel("Rz(Delta)");

    rZxAlpEd->setMaximumWidth(60);
    rZxBetEd->setMaximumWidth(60);
    rZxGamEd->setMaximumWidth(60);
    rZxDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rZXALab, 1, 2);
    rzLay->addWidget(rZxAlpEd, 1, 3);
    rzLay->addWidget(rZXBLab, 2, 2);
    rzLay->addWidget(rZxBetEd, 2, 3);
    rzLay->addWidget(rZXGLab, 3, 2);
    rzLay->addWidget(rZxGamEd, 3, 3);
    rzLay->addWidget(rZXDLab, 4, 2);
    rzLay->addWidget(rZxDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeXyWid() {
    rXyAlpEd = new QLineEdit("0");
    rXyBetEd = new QLineEdit("0");
    rXyGamEd = new QLineEdit("0");
    rXyDelEd = new QLineEdit("0");

    auto *rXYALab = new QLabel("Alpha");
    auto *rXYBLab = new QLabel("Rx(Beta)");
    auto *rXYGLab = new QLabel("Ry(Gamma)");
    auto *rXYDLab = new QLabel("Rx(Delta)");

    rXyAlpEd->setMaximumWidth(60);
    rXyBetEd->setMaximumWidth(60);
    rXyGamEd->setMaximumWidth(60);
    rXyDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rXYALab, 1, 2);
    rzLay->addWidget(rXyAlpEd, 1, 3);
    rzLay->addWidget(rXYBLab, 2, 2);
    rzLay->addWidget(rXyBetEd, 2, 3);
    rzLay->addWidget(rXYGLab, 3, 2);
    rzLay->addWidget(rXyGamEd, 3, 3);
    rzLay->addWidget(rXYDLab, 4, 2);
    rzLay->addWidget(rXyDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeZyxWid() {
    rZyxAlpEd = new QLineEdit("0");
    rZyxBetEd = new QLineEdit("0");
    rZyxGamEd = new QLineEdit("0");
    rZyxDelEd = new QLineEdit("0");

    auto *rZyxALab = new QLabel("Alpha");
    auto *rZyxBLab = new QLabel("Rz(Beta)");
    auto *rZyxGLab = new QLabel("Ry(Gamma)");
    auto *rZyxDLab = new QLabel("Rx(Delta)");

    rZyxAlpEd->setMaximumWidth(60);
    rZyxBetEd->setMaximumWidth(60);
    rZyxGamEd->setMaximumWidth(60);
    rZyxDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rZyxALab, 1, 2);
    rzLay->addWidget(rZyxAlpEd, 1, 3);
    rzLay->addWidget(rZyxBLab, 2, 2);
    rzLay->addWidget(rZyxBetEd, 2, 3);
    rzLay->addWidget(rZyxGLab, 3, 2);
    rzLay->addWidget(rZyxGamEd, 3, 3);
    rzLay->addWidget(rZyxDLab, 4, 2);
    rzLay->addWidget(rZyxDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

// TODO split function
QWidget *MainWindow::makeOpWid() {
    auto *opW = new QWidget();

    auto *qfOpButtons = new QFrame(opW);
    qfOpButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfOpButtons->move(0, 0);
    qfOpButtons->setFixedSize(opW->size());

    stackW = new QTabWidget();
    stackW->setFixedHeight(120);

    auto *bOp = new QWidget();
    auto *bOpLay = new QGridLayout();
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

    auto *mOp = new QWidget();
    auto *mOpLay = new QGridLayout();

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            mat[i][j] = new QLineEdit(QString::number(i == j));
            mat[i][j]->setValidator(&compValid);
            connect(mat[i][j], SIGNAL(textEdited(QString)),
                    SLOT(slotComplexLineEditChanged(QString)));
        }
    }

    // TODO move connectors in one function to one place or get rid of qt5 connectors
    auto *applyMat = new QPushButton("Ok");
    connect(applyMat, SIGNAL(clicked()), SLOT(slotSetMatrixOp()));

    auto *getRandomBut = new QPushButton("Random");
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

    auto *rNw = new QWidget();
    axRnEd = new QLineEdit("0;0;0");
    axRnEd->setValidator(&axisValid);
    ngRnEd = new QLineEdit("0");
    ngRnEd->setValidator(new QDoubleValidator);

    auto *axOpBut = new QPushButton("Ok");
    connect(axOpBut, SIGNAL(clicked()), SLOT(slotSetAxOp()));

    auto *rNwLay = new QGridLayout();
    rNwLay->addWidget(new QLabel("Vector"), 0, 0);
    rNwLay->addWidget(axRnEd, 0, 1, 1, 3);
    rNwLay->addWidget(new QLabel("Angle"), 1, 0);
    rNwLay->addWidget(ngRnEd, 1, 1, 1, 3);
    rNwLay->addWidget(axOpBut, 2, 3, 1, 1); // 4
    rNwLay->setContentsMargins(20, 10, 20, 0);
    rNwLay->setSpacing(5);
    rNw->setLayout(rNwLay);

    stackW->addTab(rNw, "Rn");

    auto *qGb = new QGroupBox("Rotation");

    rZyRb = new QRadioButton("ZY-decomposition");
    rZyRb->toggle();

    rZxRb = new QRadioButton("ZX-decomposition");
    rXyRb = new QRadioButton("XY-decomposition");
    rZyxRb = new QRadioButton("ZYX-decomposition");
    rtRb = new QRadioButton("Rotation about vector");

    connect(rtRb, SIGNAL(toggled(bool)), SLOT(slotToggleRotateVector(bool)));

    auto *gbLay = new QVBoxLayout();
    qGb->setFixedHeight(120);
    gbLay->addWidget(rZyRb);
    gbLay->addWidget(rZxRb);
    gbLay->addWidget(rXyRb);
    gbLay->addWidget(rZyxRb);
    gbLay->addWidget(rtRb);
    gbLay->setSpacing(5);
    qGb->setLayout(gbLay);

    appBut = new QPushButton("Apply Id");
    appBut->setToolTip(curOperator.getCurOperatorMatrixStr());
    appBut->setFixedHeight(35);
    connect(appBut, SIGNAL(clicked()), SLOT(slotApplyOp()));

    auto *addToQueBut = new QPushButton("Add to queue");
    addToQueBut->setFixedHeight(35);
    connect(addToQueBut, SIGNAL(clicked()), SLOT(slotAddToQue()));

    auto *horLay = new QHBoxLayout;
    horLay->addWidget(appBut);
    horLay->addWidget(addToQueBut);

    auto *pLay = new QVBoxLayout();
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
    auto *newOpBut = new QPushButton(str);
    newOpBut->setFixedHeight(26);
    connect(newOpBut, SIGNAL(clicked()), SLOT(slotSetOperatorClicked()));
    return newOpBut;
}

void MainWindow::slotSaveState() {
    removeAllVectors(savedVectors);
    foreach (auto &e, vectors.keys()) { savedVectors.insert(e->getCopyState(), vectors[e]); }
    recallState->setEnabled(true);
}

void MainWindow::slotRecallState() {
    removeAllVectors(vectors);
    foreach (auto &e, savedVectors.keys()) { addVector(e->getCopyState(), vectors); }
    foreach (auto e, topTabWid->findChildren<VectorWidget *>()) {
        if (e->getVector() != nullptr) {
            e->fillFieldsOfVector(e->getVector()->getSpike());
        }
    }
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
        dec.alpha = rZyAlpEd->text().toDouble();
        dec.beta = rZyBetEd->text().toDouble();
        dec.gamma = rZyGamEd->text().toDouble();
        dec.delta = rZyDelEd->text().toDouble();
        if (not curOperator.setOperatorByZyDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(this, "Error", "error");
            return;
        }
        break;
    case 1:
        dec.alpha = rZxAlpEd->text().toDouble();
        dec.beta = rZxBetEd->text().toDouble();
        dec.gamma = rZxGamEd->text().toDouble();
        dec.delta = rZxDelEd->text().toDouble();
        if (not curOperator.setOperatorByZxDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(this, "Error", "error");
            return;
        }
        break;
    case 2:
        dec.alpha = rXyAlpEd->text().toDouble();
        dec.beta = rXyBetEd->text().toDouble();
        dec.gamma = rXyGamEd->text().toDouble();
        dec.delta = rXyDelEd->text().toDouble();
        if (not curOperator.setOperatorByXyDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(this, "Error", "error");
            return;
        }
        break;
    case 3:
        dec.alpha = rZyxAlpEd->text().toDouble();
        dec.beta = rZyxBetEd->text().toDouble();
        dec.gamma = rZyxGamEd->text().toDouble();
        dec.delta = rZyxDelEd->text().toDouble();
        if (not curOperator.setOperatorByZyxDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(this, "Error", "error");
            return;
        }
        break;
    }

    updateOp();
}

void MainWindow::slotSetMatrixOp() {
    QVector<complex> res;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            try {
                res.push_back(parseStrToComplex(mat[i][j]->text()));
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
        if (abs(va.x) + abs(va.y) + abs(va.z) < EPSILON) {
            QMessageBox::warning(this, "Warning", "Vector must be non-zero");
            return;
        }

        double len = sqrt(va.x * va.x + va.y * va.y + va.z * va.z);
        if (not UnitaryMatrix2x2::fuzzyCompare(len, 1.)) {
            // TODO  (QWidget *)sender() ? qt4 fails with "this"
            auto *dial = new BlochDialog((QWidget *)sender(), DIALOG_TYPE::NORMALIZE);
            if (isAutoNormalize or dial->exec() == QDialog::Accepted) {
                va.x /= len;
                va.y /= len;
                va.z /= len;
                axRnEd->setText(QString("%1;%2;%3")
                                    .arg(roundNumber(va.x))
                                    .arg(roundNumber(va.y))
                                    .arg(roundNumber(va.z)));
            } else {
                return;
            }
        }

        if (not curOperator.setOperatorByVectorAngle(va)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(this, "Error", "error");
            return;
        }

        updateOp(OPERATOR_FORM::VECTOR);
    } else {
        QMessageBox::warning(this, "Error", "Wrong input: Vector (x;y;z)");
        return;
    }
}

void MainWindow::updateOp(OPERATOR_FORM exclude) {
    appBut->setText("Apply " + curOperator.getOperatorName());

    decomposition zyDec = curOperator.zyDecomposition();
    rZyAlpEd->setText(numberToStr(zyDec.alpha));
    rZyBetEd->setText(numberToStr(zyDec.beta));
    rZyGamEd->setText(numberToStr(zyDec.gamma));
    rZyDelEd->setText(numberToStr(zyDec.delta));

    decomposition zxDec = curOperator.zxDecomposition();
    rZxAlpEd->setText(numberToStr(zxDec.alpha));
    rZxBetEd->setText(numberToStr(zxDec.beta));
    rZxGamEd->setText(numberToStr(zxDec.gamma));
    rZxDelEd->setText(numberToStr(zxDec.delta));

    decomposition xyDec = curOperator.xyDecomposition();
    rXyAlpEd->setText(numberToStr(xyDec.alpha));
    rXyBetEd->setText(numberToStr(xyDec.beta));
    rXyGamEd->setText(numberToStr(xyDec.gamma));
    rXyDelEd->setText(numberToStr(xyDec.delta));

    decomposition zyxDec = curOperator.zyxDecomposition();
    rZyxAlpEd->setText(numberToStr(zyxDec.alpha));
    rZyxBetEd->setText(numberToStr(zyxDec.beta));
    rZyxGamEd->setText(numberToStr(zyxDec.gamma));
    rZyxDelEd->setText(numberToStr(zyxDec.delta));

    if (exclude != OPERATOR_FORM::MATRIX) {
        mat[0][0]->setText(parseComplexToStr(curOperator.getOperator().a()));
        mat[0][1]->setText(parseComplexToStr(curOperator.getOperator().b()));
        mat[1][0]->setText(parseComplexToStr(curOperator.getOperator().c()));
        mat[1][1]->setText(parseComplexToStr(curOperator.getOperator().d()));
        appBut->setToolTip(curOperator.getCurOperatorMatrixStr());
    }

    vectorangle va = curOperator.vectorAngleDec();
    foreach (auto e, vectors.keys()) { e->setRotateVector(QVector3D(va.x, va.y, va.z)); }

    if (exclude != OPERATOR_FORM::VECTOR) {
        axRnEd->setText(QString("%1;%2;%3")
                            .arg(roundNumber(va.x))
                            .arg(roundNumber(va.y))
                            .arg(roundNumber(va.z)));
        ngRnEd->setText(QString("%1").arg(roundNumber(va.angle * 180 / M_PI)));
    }

    slotUpdateSpheres();
}

void MainWindow::slotQueItemClicked(QListWidgetItem *it) {
    auto *opi = (OpItem *)it;
    curOperator = opi->getOp();
    updateOp();
}

void MainWindow::slotOpItemDelete() {
    if (!opQueWid->selectedItems().isEmpty())
        delete opQueWid->currentItem();
}

void MainWindow::slotAddToQue() {
    auto *it = new OpItem(curOperator.getOperatorName(), curOperator);
    opQueWid->insertItem(0, it);
}

void MainWindow::slotApplyOp() {
    stopTimer();
    if (curOperator.getOperatorName() == "Id") {
        return;
    }
    foreach (auto e, vectors.keys()) { startMove(e, getCurrentDecomposition()); }
}

void MainWindow::slotApplyQue() {
    stopTimer();
    isQueueAnimation = true;
    singleOperator = curOperator;
    for (int i = 0; i < opQueWid->count(); ++i) {
        opQueue.append((OpItem *)(opQueWid->item(i)));
    }

    if (not opQueue.isEmpty()) {
        curOperator = opQueue.last()->getOp();
        opQueue.last()->setBackground(QBrush(Qt::red));
        updateOp();
        slotApplyOp();
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

    foreach (auto e, circuit->getQubits()) {
        startMove(e->getVector(), e->getOperator(circuitStepNumber), getCurrentDecomposition());
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

void MainWindow::slotAbout() { QMessageBox::about(this, "About program", "blochsphere"); }

// TODO check
void MainWindow::slotReset() {
    stopTimer();
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

    slotPlusSphere();
}

void MainWindow::slotComplexLineEditChanged(const QString &) {
    updateComplexLineEdit(mat[0][0]);
    updateComplexLineEdit(mat[0][1]);
    updateComplexLineEdit(mat[1][0]);
    updateComplexLineEdit(mat[1][1]);
}

void MainWindow::updateComplexLineEdit(QLineEdit *lineEdit) {
    QRegExp re(QString::fromUtf8("[IШш]"));
    lineEdit->setText(lineEdit->text().replace(re, "i"));
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
    circuit->addStepBut->setEnabled(f and circuit->getQubits().size() < MAX_COUNT_OF_STEPS);
    circuit->removeStepBut->setEnabled(f and circuit->getQubits().size() > 1);
}

void MainWindow::slotStartCircuitMove() {
    circuitStepNumber = 0;
    isCircuitAnimation = true;
    nextAnimStepCircuit();
}

void MainWindow::slotUpdateSpheres() {
    foreach (auto e, spheres) { e->update(); }
}

void MainWindow::slotSpeedUp() {
    if (getSpeed() < 10) {
        setSpeed(getSpeed() + 1);
    }
    speedLabel->setText("Speed: " + QString::number(getSpeed()) + " ");
    speedUpButton->setEnabled(getSpeed() < 10);
    speedDownButton->setEnabled(getSpeed() > 1);
}

void MainWindow::slotSpeedDown() {
    if (getSpeed() > 0) {
        setSpeed(getSpeed() - 1);
    }
    speedLabel->setText("Speed: " + QString::number(getSpeed()) + " ");
    speedUpButton->setEnabled(getSpeed() < 10);
    speedDownButton->setEnabled(getSpeed() > 1);
}
