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
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QString>
#include <QTime>
#include <QToolBar>

// TODO move to another place
QRegExpValidator compValid(QRegExp("^[+-]?[0-9]*\\.?[0-9]*[+-]?[0-9]*\\.?[0-9]*i?$"));
QRegExpValidator axisValid(QRegExp("^-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*$"));

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent} {
    createActions();
    createMenu();
    createStatusBar();
    createTopBar();
    createSideWidget();
    createOpQueWidget();
    createSphere();
    addVector(new Vector(0., 0.), vectors);
}

void MainWindow::addVector(Vector *v, MapVectors &mp) {
    mp.insert(v, QVector<Sphere *>());

    for (auto &e : spheres) {
        e->addVector(v);
        mp[v].append(e);
    }
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

void MainWindow::timerEvent(QTimerEvent *timerEvent) {
    bool isNowAnimate = false;
    foreach (auto &e, vectors.keys()) {
        isNowAnimate |= e->isNowAnimate();
        statusBar()->showMessage(QString::number(e->isNowAnimate()) +
                                 QString::number(isNowAnimate) + QTime::currentTime().toString());
        fillFieldsOfVector(e->getSpike());
    }

    if (not isNowAnimate) {
        this->killTimer(timerEvent->timerId());
        appBut->setEnabled(true);
    }
}

void MainWindow::createSphere() {
    controlWidget = new QWidget(this);
    setCentralWidget(controlWidget);
    auto *layout = new QGridLayout(controlWidget);
    for (int i = 0; i < 1; ++i) {
        for (int j = 1; j < 2; ++j) {
            spheres.append(new Sphere(controlWidget, QString::number(i + 10 * j)));
            layout->addWidget(spheres.last(), i, j);
        }
    }
    controlWidget->setFocus();
}

//    QRandomGenerator64 rd(seed);
void MainWindow::createActions() {
    aboutAct = new QAction("About program", this);
#if QT_VERSION >= 0x050000
    connect(aboutAct, &QAction::triggered, this, &MainWindow::slotAbout);
#else
    connect(aboutAct, SIGNAL(triggered()), SLOT(slotAbout()));
#endif

    saveState = new QAction("Save state", this);
#if QT_VERSION >= 0x050000
    connect(saveState, &QAction::triggered, this, &MainWindow::slotSaveState);
#else
    connect(saveState, SIGNAL(triggered()), SLOT(slotSaveState()));
#endif

    recallState = new QAction("Recall state", this);
    recallState->setEnabled(false);
#if QT_VERSION >= 0x050000
    connect(recallState, &QAction::triggered, this, &MainWindow::slotRecallState);
#else
    connect(recallState, SIGNAL(triggered()), SLOT(slotRecallState()));
#endif

    resetAct = new QAction("Reset", this);
#if QT_VERSION >= 0x050000
    connect(resetAct, &QAction::triggered, this, &MainWindow::slotReset);
#else
    connect(resetAct, SIGNAL(triggered()), SLOT(slotReset()));
#endif

    showTAct = new QAction("Show trace", this);
    showTAct->setCheckable(true);
    showTAct->setChecked(true);
#if QT_VERSION >= 0x050000
    connect(showTAct, &QAction::triggered, this, &MainWindow::slotShowTrace);
#else
    connect(showTAct, SIGNAL(triggered()), SLOT(slotShowTrace()));
#endif

    clearTAct = new QAction("Clear trace", this);
#if QT_VERSION >= 0x050000
    connect(clearTAct, &QAction::triggered, this, &MainWindow::slotClearTrace);
#else
    connect(clearTAct, SIGNAL(triggered()), SLOT(slotClearTrace()));
#endif

    exitAct = new QAction("Exit", this);
#if QT_VERSION >= 0x050000
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
#else
    connect(exitAct, SIGNAL(triggered()), SLOT(close()));
#endif
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
    qtb->addAction(saveState);
    qtb->addAction(recallState);
    qtb->addAction(resetAct);
    qtb->addSeparator();
    qtb->addAction(showTAct);
    qtb->addAction(clearTAct);

    qcomb = new QComboBox(qtb);
    qcomb->addItem("Red");
    qcomb->addItem("Green");
    qcomb->addItem("Blue");
    qcomb->addItem("Grey");
    qcomb->addItem("Yellow");
    qcomb->setCurrentIndex(0);

#if QT_VERSION >= 0x050000
    connect(qcomb, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MainWindow::slotTraceColor);
#else
    connect(qcomb, SIGNAL(currentIndexChanged(int)), SLOT(slotTraceColor(int)));
#endif

    qtb->addWidget(new QLabel("<center>Trace Color:"));
    qtb->addWidget(qcomb);

    this->addToolBar(Qt::TopToolBarArea, qtb);
}

void MainWindow::createOpQueWidget() {
    opQueWid = new QListWidget;
    opQueWid->setFlow(QListView::LeftToRight);
    opQueWid->setSpacing(6);
    connect(opQueWid, SIGNAL(itemClicked(QListWidgetItem *)),
            SLOT(slotQueItemClicked(QListWidgetItem *)));
    QAction *delAct = new QAction("Delete", opQueWid);
    opQueWid->addAction(delAct);
    connect(delAct, SIGNAL(triggered()), SLOT(slotOpItemDelete()));
    opQueWid->setContextMenuPolicy(Qt::ActionsContextMenu);

    QPushButton *appQueBut = new QPushButton("Apply queue");
    appQueBut->setFixedSize(70, 40);
    connect(appQueBut, SIGNAL(clicked()), SLOT(slotApplyQue()));
    QPushButton *clrQueBut = new QPushButton("Clear queue");
    clrQueBut->setFixedSize(70, 40);
    connect(clrQueBut, SIGNAL(clicked()), opQueWid, SLOT(clear()));

    QToolBar *qtb = new QToolBar("Operators queue");
    qtb->addWidget(opQueWid);
    qtb->addWidget(appQueBut);
    qtb->addWidget(clrQueBut);
    qtb->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
    qtb->setFixedHeight(40);
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
    auto *leftWid = new QWidget();
    auto *mainLay = new QVBoxLayout();

    auto *topWid = new QWidget();
    auto *topLay = new QHBoxLayout();
    topLay->addWidget(makeThePhiWid());
    topLay->addWidget(makeAlpBetWid());
    topLay->addWidget(makeBloVecWid());
    topLay->setMargin(0);
    topLay->setSpacing(0);
    topWid->setLayout(topLay);

    mainLay->addWidget(topWid);
    mainLay->addWidget(makeRXYZWid());
    mainLay->addWidget(makeOpWid());
    mainLay->setSpacing(0);
    mainLay->setMargin(0);
    leftWid->setLayout(mainLay);
    leftWid->setFixedWidth(400);

    auto *qtb = new QToolBar("Control panel");
    qtb->addWidget(leftWid);
    qtb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    this->addToolBar(Qt::LeftToolBarArea, qtb);
}

QWidget *MainWindow::makeThePhiWid() {
    theEd = new QLineEdit("0");
    phiEd = new QLineEdit("0");
    auto *theLab = new QLabel("<font face=symbol size=5>q</font>");
    auto *phiLab = new QLabel("<font face=symbol size=5>f</font>");

    theEd->setMaximumWidth(55);
    theEd->setValidator(new QDoubleValidator);

    phiEd->setMaximumWidth(55);
    phiEd->setValidator(new QDoubleValidator);

    auto *thePhiButton = new QPushButton("Set");
    thePhiButton->setFixedWidth(55);
#if QT_VERSION >= 0x050000
    connect(thePhiButton, &QPushButton::clicked, this, &MainWindow::slotThePhi);
#else
    connect(thePhiButton, SIGNAL(clicked()), SLOT(slotThePhi()));
#endif

    auto *tpW = new QWidget();
    tpW->setFixedHeight(90);

    auto *qfThePhi = new QFrame(tpW);
    qfThePhi->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfThePhi->move(0, 0);
    qfThePhi->setFixedSize(tpW->size());

    auto *tpLay = new QGridLayout();
    tpLay->addWidget(theLab, 0, 0);
    tpLay->addWidget(theEd, 0, 1, 1, 3);
    tpLay->addWidget(phiLab, 1, 0);
    tpLay->addWidget(phiEd, 1, 1, 1, 3);
    tpLay->addWidget(thePhiButton, 4, 1, 1, 2);
    tpLay->setSpacing(2);
    tpLay->setContentsMargins(5, 5, 5, 5);
    tpW->setLayout(tpLay);

    return tpW;
}

QWidget *MainWindow::makeAlpBetWid() {
    alpEd = new QLineEdit("1");
    reBetEd = new QLineEdit("0");
    auto *alpLab = new QLabel("<font face=symbol size=5>a</font>");
    auto *reBetLab = new QLabel("<font face=symbol size=5>b</font>");
    auto *imBetLab = new QLabel("<font size=4>Im(<font face=symbol size=5>b</font>)</font>");

    alpEd->setFixedWidth(90);
    alpEd->setValidator(new QDoubleValidator);

    reBetEd->setFixedWidth(90);
    reBetEd->setValidator(&compValid);

    auto *bPsi = new QPushButton("Set");
    bPsi->setFixedWidth(60);
#if QT_VERSION >= 0x050000
    connect(bPsi, &QPushButton::clicked, this, &MainWindow::slotAlpBet);
#else
    connect(bPsi, SIGNAL(clicked()), SLOT(slotAlpBet()));
#endif

    auto *bRandPsi = new QPushButton("Random");
    bRandPsi->setFixedWidth(60);
#if QT_VERSION >= 0x050000
    connect(bRandPsi, &QPushButton::clicked, this, &MainWindow::slotSetRandomPsi);
#else
    connect(bRandPsi, SIGNAL(clicked()), SLOT(slotSetRandomPsi()));
#endif

    auto *abW = new QWidget();
    abW->setFixedHeight(90);

    auto *qfAlpBet = new QFrame(abW);
    qfAlpBet->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfAlpBet->move(0, 0);
    qfAlpBet->setFixedSize(abW->size());

    auto *abLay = new QGridLayout();
    abLay->addWidget(alpLab, 1, 0);
    abLay->addWidget(alpEd, 1, 1, 1, 3);
    abLay->addWidget(reBetLab, 2, 0);
    abLay->addWidget(reBetEd, 2, 1, 1, 3);
    abLay->addWidget(bRandPsi, 4, 0, 1, 2);
    abLay->addWidget(bPsi, 4, 2);
    abLay->setContentsMargins(10, 5, 5, 5);
    abLay->setSpacing(2);
    abW->setLayout(abLay);

    return abW;
}

QWidget *MainWindow::makeBloVecWid() {
    xEd = new QLineEdit("0");
    yEd = new QLineEdit("0");
    zEd = new QLineEdit("1");

    auto *xLab = new QLabel("<font size=4>x</font>");
    auto *yLab = new QLabel("<font size=4>y</font>");
    auto *zLab = new QLabel("<font size=4>z</font>");

    xEd->setFixedWidth(90);
    xEd->setValidator(new QDoubleValidator);
    yEd->setFixedWidth(90);
    yEd->setValidator(new QDoubleValidator);
    zEd->setFixedWidth(90);
    zEd->setValidator(new QDoubleValidator);

    auto *bPsi = new QPushButton("Set");
    bPsi->setFixedWidth(60);
#if QT_VERSION >= 0x050000
    connect(bPsi, &QPushButton::clicked, this, &MainWindow::slotBloVec);
#else
    connect(bPsi, SIGNAL(clicked()), SLOT(slotBloVec()));
#endif

    auto *xyzW = new QWidget();
    xyzW->setFixedHeight(90);

    auto *qfAlpBet = new QFrame(xyzW);
    qfAlpBet->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfAlpBet->move(0, 0);
    qfAlpBet->setFixedSize(xyzW->size());

    auto *abLay = new QGridLayout();
    abLay->addWidget(xLab, 1, 0);
    abLay->addWidget(xEd, 1, 1, 1, 3);
    abLay->addWidget(yLab, 2, 0);
    abLay->addWidget(yEd, 2, 1, 1, 3);
    abLay->addWidget(zLab, 3, 0);
    abLay->addWidget(zEd, 3, 1, 1, 3);
    abLay->addWidget(bPsi, 4, 2);

    abLay->setContentsMargins(10, 5, 5, 5);
    abLay->setSpacing(2);
    xyzW->setLayout(abLay);

    return xyzW;
}

QWidget *MainWindow::makeRXYZWid() {
    rxyzTab = new QTabWidget;
    rxyzTab->insertTab(0, makeRZYWid(), "Z-Y");
    rxyzTab->insertTab(1, makeRZXWid(), "Z-X");
    rxyzTab->insertTab(2, makeRXYWid(), "X-Y");
    rxyzTab->setCurrentIndex(0);

    auto *bRotXYZ = new QPushButton("Set");
    bRotXYZ->setFixedWidth(60);
#if QT_VERSION >= 0x050000
    connect(bRotXYZ, &QPushButton::clicked, this, &MainWindow::slotSetRXYZOp);
#else
    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));
#endif

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

QWidget *MainWindow::makeRZYWid() {
    rZYAlpEd = new QLineEdit("0");
    rZYBetEd = new QLineEdit("0");
    rZYGamEd = new QLineEdit("0");
    rZYDelEd = new QLineEdit("0");

    auto *rZYALab = new QLabel("Alpha");
    auto *rZYBLab = new QLabel("Rz(Beta)");
    auto *rZYGLab = new QLabel("Ry(Gamma)");
    auto *rZYDLab = new QLabel("Ry(Delta)");

    rZYAlpEd->setMaximumWidth(60);
    rZYBetEd->setMaximumWidth(60);
    rZYGamEd->setMaximumWidth(60);
    rZYDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rZYALab, 1, 2);
    rzLay->addWidget(rZYAlpEd, 1, 3);
    rzLay->addWidget(rZYBLab, 2, 2);
    rzLay->addWidget(rZYBetEd, 2, 3);
    rzLay->addWidget(rZYGLab, 3, 2);
    rzLay->addWidget(rZYGamEd, 3, 3);
    rzLay->addWidget(rZYDLab, 4, 2);
    rzLay->addWidget(rZYDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeRZXWid() {
    rZXAlpEd = new QLineEdit("0");
    rZXBetEd = new QLineEdit("0");
    rZXGamEd = new QLineEdit("0");
    rZXDelEd = new QLineEdit("0");

    auto *rZXALab = new QLabel("Alpha");
    auto *rZXBLab = new QLabel("Rz(Beta)");
    auto *rZXGLab = new QLabel("Rx(Gamma)");
    auto *rZXDLab = new QLabel("Rz(Delta)");

    rZXAlpEd->setMaximumWidth(60);
    rZXBetEd->setMaximumWidth(60);
    rZXGamEd->setMaximumWidth(60);
    rZXDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rZXALab, 1, 2);
    rzLay->addWidget(rZXAlpEd, 1, 3);
    rzLay->addWidget(rZXBLab, 2, 2);
    rzLay->addWidget(rZXBetEd, 2, 3);
    rzLay->addWidget(rZXGLab, 3, 2);
    rzLay->addWidget(rZXGamEd, 3, 3);
    rzLay->addWidget(rZXDLab, 4, 2);
    rzLay->addWidget(rZXDelEd, 4, 3);
    rzLay->setContentsMargins(20, 5, 20, 5);
    rzLay->setSpacing(2);
    rzW->setLayout(rzLay);

    return rzW;
}

QWidget *MainWindow::makeRXYWid() {
    rXYAlpEd = new QLineEdit("0");
    rXYBetEd = new QLineEdit("0");
    rXYGamEd = new QLineEdit("0");
    rXYDelEd = new QLineEdit("0");

    auto *rXYALab = new QLabel("Alpha");
    auto *rXYBLab = new QLabel("Rx(Beta)");
    auto *rXYGLab = new QLabel("Ry(Gamma)");
    auto *rXYDLab = new QLabel("Rx(Delta)");

    rXYAlpEd->setMaximumWidth(60);
    rXYBetEd->setMaximumWidth(60);
    rXYGamEd->setMaximumWidth(60);
    rXYDelEd->setMaximumWidth(60);

    auto *rzW = new QWidget();

    auto *rzLay = new QGridLayout();
    rzLay->addWidget(rXYALab, 1, 2);
    rzLay->addWidget(rXYAlpEd, 1, 3);
    rzLay->addWidget(rXYBLab, 2, 2);
    rzLay->addWidget(rXYBetEd, 2, 3);
    rzLay->addWidget(rXYGLab, 3, 2);
    rzLay->addWidget(rXYGamEd, 3, 3);
    rzLay->addWidget(rXYDLab, 4, 2);
    rzLay->addWidget(rXYDelEd, 4, 3);
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
        }
    }

    auto *applyMat = new QPushButton("Ok");
#if QT_VERSION >= 0x050000
    connect(applyMat, &QPushButton::clicked, this, &MainWindow::slotSetMatrixOp);
#else
    connect(applyMat, SIGNAL(clicked()), SLOT(slotSetMatrixOp()));
#endif

    mOpLay->addWidget(mat[0][0], 1, 0);
    mOpLay->addWidget(mat[0][1], 1, 5);
    mOpLay->addWidget(mat[1][0], 2, 0);
    mOpLay->addWidget(mat[1][1], 2, 5);
    mOpLay->addWidget(applyMat, 3, 5);
    mOpLay->setContentsMargins(10, 10, 10, 0);
    mOp->setLayout(mOpLay);

    stackW->addTab(mOp, "Matrix");

    auto *rNw = new QWidget();
    axRnEd = new QLineEdit();
    axRnEd->setValidator(&axisValid);
    ngRnEd = new QLineEdit();
    ngRnEd->setValidator(new QDoubleValidator);

    auto *axOpBut = new QPushButton("Ok");
#if QT_VERSION >= 0x050000
    connect(axOpBut, &QPushButton::clicked, this, &MainWindow::slotSetNewAxOp);
#else
    connect(axOpBut, SIGNAL(clicked()), SLOT(slotSetNewAxOp()));
#endif

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

    rzyRB = new QRadioButton("ZY-decomposition");
    rzyRB->toggle();

    rzxRB = new QRadioButton("ZX-decomposition");

    rxyRB = new QRadioButton("XY-decomposition");

    rtRB = new QRadioButton("Rotation about vector");

    auto *gbLay = new QVBoxLayout();
    qGb->setFixedHeight(120);
    gbLay->addWidget(rzyRB);
    gbLay->addWidget(rzxRB);
    gbLay->addWidget(rxyRB);
    gbLay->addWidget(rtRB);
    gbLay->setSpacing(5);
    qGb->setLayout(gbLay);

    appBut = new QPushButton("Apply operator");
    appBut->setFixedHeight(35);
#if QT_VERSION >= 0x050000
    connect(appBut, &QPushButton::clicked, this, &MainWindow::slotApplyOp);
#else
    connect(appBut, SIGNAL(clicked()), SLOT(slotApplyOp()));
#endif
    auto *addToQueBut = new QPushButton("Add to queue");
    addToQueBut->setFixedHeight(35);
#if QT_VERSION >= 0x050000
    connect(addToQueBut, &QPushButton::clicked, this, &MainWindow::slotAddToQue);
#else
    connect(addToQueBut, SIGNAL(clicked()), SLOT(slotAddToQue()));
#endif

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

QPushButton *MainWindow::makeOpButton(QString str) {
    auto *newOpBut = new QPushButton(str);
    newOpBut->setFixedHeight(26);
#if QT_VERSION >= 0x050000
    connect(newOpBut, &QPushButton::clicked, this, &MainWindow::slotSetOperatorClicked);
#else
    connect(newOpBut, SIGNAL(clicked()), SLOT(slotSetOperatorClicked()));
#endif
    return newOpBut;
}

// TODO add check ranges
// DOTO push while animating
void MainWindow::slotThePhi() {
    double the = qDegreesToRadians(theEd->text().toDouble());
    double phi = qDegreesToRadians(phiEd->text().toDouble());
    Spike  sp = Vector::createSpike(the, phi);
    foreach (auto &e, vectors.keys()) { e->changeVector(sp); }

    fillFieldsOfVector(sp, FIELD::THEPHI);
}

// TODO add check if normalized
// DOTO push while animating
void MainWindow::slotAlpBet() {
    double  a = alpEd->text().toDouble();
    complex b = parseStrToComplex(reBetEd->text());
    Spike   sp = Vector::createSpike(a, b);
    foreach (auto &e, vectors.keys()) { e->changeVector(sp); }

    fillFieldsOfVector(sp, FIELD::ALPBET);
}

void MainWindow::slotBloVec() {
    double x = xEd->text().toDouble();
    double y = yEd->text().toDouble();
    double z = zEd->text().toDouble();
    Spike  sp = Vector::createSpike(x, y, z);
    foreach (auto &e, vectors.keys()) { e->changeVector(sp); }

    fillFieldsOfVector(sp, FIELD::BLOVEC);
}

void MainWindow::slotSetRandomPsi() {
    double the = random(0, 180);
    double phi = random(0, 360);

    Spike sp = Vector::createSpike(the, phi);
    foreach (auto &e, vectors.keys()) { e->changeVector(sp); }

    fillFieldsOfVector(sp);
}

void MainWindow::slotSaveState() {
    removeAllVectors(savedVectors);
    foreach (auto &e, vectors.keys()) { savedVectors.insert(e->getCopyState(), vectors[e]); }
    recallState->setEnabled(true);
}

void MainWindow::slotRecallState() {
    removeAllVectors(vectors);
    vectors.clear();
    foreach (auto &e, savedVectors.keys()) { addVector(e->getCopyState(), vectors); }
    fillFieldsOfVector(vectors.keys().last()->getSpike());
}

void MainWindow::slotMotionBegin(QString msg) {
    mtnStLab->setText("<font size=4>" + msg + "</font>");
    mtnStLab->setVisible(true);
}

void MainWindow::slotMotionEnd() { mtnStLab->setVisible(false); }

void MainWindow::slotShowTrace() {
    foreach (auto &e, vectors.keys()) { e->setEnableTrace(showTAct->isChecked()); }
}

void MainWindow::slotClearTrace() {
    foreach (auto &e, vectors.keys()) { e->clearTrace(); }
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
    std::string str = ((QPushButton *)sender())->text().toStdString();
    //    AngInput   *aIn;
    //    QString     axSt = axRnEd->text();
    //    bool        rzyCond = rzyRB->isChecked();

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
        //            case 'P':
        //                aIn = new AngInput((QWidget *)sender());
        //                if (aIn->exec() == QDialog::Accepted)
        //                    op = PHIop(aIn->ang().toFloat() * RAD);
        //                else {
        //                    delete aIn;
        //                    return;
        //                }
        //                delete aIn;
        //                break;
        //            case 'R':
        //                aIn = new AngInput((QWidget *)sender());
        //                if (aIn->exec() == QDialog::Accepted) {
        //                    float the = (aIn->ang().toFloat()) * RAD;
        //                    if (str[1] == 'x')
        //                        op = RXop(the);
        //                    else if (str[1] == 'y')
        //                        op = RYop(the);
        //                    else if (str[1] == 'z')
        //                        op = RZop(the);
        //
        //                } else {
        //                    delete aIn;
        //                    return;
        //                }
        //                delete aIn;
        //                break;
    }
    curOpName = str.c_str();
    updateOp();
}

void MainWindow::slotNewOp(Operator &op) {
    curOperator = op;
    updateOp();
}

void MainWindow::slotSetRXYZOp() {
    decomposition dec;
    switch (rxyzTab->currentIndex()) {
    case 0:
        dec.alpha = rZYAlpEd->text().toDouble();
        dec.beta = rZYBetEd->text().toDouble();
        dec.gamma = rZYGamEd->text().toDouble();
        dec.delta = rZYDelEd->text().toDouble();
        if (not curOperator.setOperatorByZYDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(0, "Error", "error");
            return;
        }
        break;
    case 1:
        dec.alpha = rZXAlpEd->text().toDouble();
        dec.beta = rZXBetEd->text().toDouble();
        dec.gamma = rZXGamEd->text().toDouble();
        dec.delta = rZXDelEd->text().toDouble();
        if (not curOperator.setOperatorByZXDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(0, "Error", "error");
            return;
        }
        break;
    case 2:
        dec.alpha = rXYAlpEd->text().toDouble();
        dec.beta = rXYBetEd->text().toDouble();
        dec.gamma = rXYGamEd->text().toDouble();
        dec.delta = rXYDelEd->text().toDouble();
        if (not curOperator.setOperatorByXYDecomposition(dec)) {
            // TODO it's impossible; inspect
            QMessageBox::warning(0, "Error", "error");
            return;
        }
        break;
    }
    curOpName = "U";
    updateOp();
}

void MainWindow::slotSetMatrixOp() {
    QVector<complex> res;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            try {
                res.push_back(parseStrToComplex(mat[i][j]->text()));
            } catch (int e) {
                QMessageBox::warning(0, "Error",
                                     QString("Wrong input: Matrix(%1,%2)").arg(i).arg(j));
                return;
            }
        }
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix({res[0], res[1], res[2], res[3]})) {
        QMessageBox::warning(0, "Error", "Matrix must be unitary");
        return;
    }
    curOpName = "U";
    curOperator.setOperator(matrixOp);

    // TODO may change matrix fields
    updateOp();
    //    slotNewOp(op);
}

void MainWindow::slotSetNewAxOp() {
    QRegExp rxp("^(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?"
                "[\\d]*e?[+-]?[\\d]*)$");
    //    if (axRnEd->text().contains(rxp)) {
    //        double ng = ngRnEd->text().toDouble() * RAD, nX = rxp.capturedTexts()[1].toDouble(),
    //               nY = rxp.capturedTexts()[2].toDouble(), nZ = rxp.capturedTexts()[3].toDouble();
    //        // 4
    //        double k = (nX < 0) ? -1.0 : 1.0, l = (nY < 0) ? -1.0 : 1.0, m = (nZ < 0) ? -1.0
    //        : 1.0; double n = nX * nX + nY * nY + nZ * nZ;
    //
    //        nX = k * sqrt(nX * nX / n);
    //        nY = l * sqrt(nY * nY / n);
    //        nZ = m * sqrt(nZ * nZ / n);
    //
    //        // scene->setNewAxis(nX, nY, nZ);
    //
    //        //        curOperator = cos(ng / 2.0) * Iop - C_I * sin(ng / 2.0) * (nX * Xop + nY *
    //        Yop +
    //        //        nZ * Zop);
    //
    //        curOpName = "U";
    //        updateOp();
    //    } else {
    //        QMessageBox::warning(0, "Error", "Wrong input: Vector (x;y;z)");
    //        return;
    //    }
}

void MainWindow::updateOp() {
    decomposition zyDec = curOperator.zyDecomposition();
    rZYAlpEd->setText(QString::number(zyDec.alpha));
    rZYBetEd->setText(QString::number(zyDec.beta));
    rZYGamEd->setText(QString::number(zyDec.gamma));
    rZYDelEd->setText(QString::number(zyDec.delta));

    decomposition zxDec = curOperator.zxDecomposition();
    rZXAlpEd->setText(QString::number(zxDec.alpha));
    rZXBetEd->setText(QString::number(zxDec.beta));
    rZXGamEd->setText(QString::number(zxDec.gamma));
    rZXDelEd->setText(QString::number(zxDec.delta));

    decomposition xyDec = curOperator.xyDecomposition();
    rXYAlpEd->setText(QString::number(xyDec.alpha));
    rXYBetEd->setText(QString::number(xyDec.beta));
    rXYGamEd->setText(QString::number(xyDec.gamma));
    rXYDelEd->setText(QString::number(xyDec.delta));

    mat[0][0]->setText(parseComplexToStr(curOperator.getOperator().a()));
    mat[0][1]->setText(parseComplexToStr(curOperator.getOperator().b()));
    mat[1][0]->setText(parseComplexToStr(curOperator.getOperator().c()));
    mat[1][1]->setText(parseComplexToStr(curOperator.getOperator().d()));

    //        QVector<double> x = curOperator.findNVec();
    //        axRnEd->setText(QString("%1;%2;%3")
    //                            .arg(roundNumber(x[0], 1000))
    //                            .arg(roundNumber(x[1], 1000))
    //                            .arg(roundNumber(x[2], 1000)));          // 4
    //        ngRnEd->setText(QString("%1").arg(roundNumber(x[3] * DEG))); // 4
}

// TODO check
void MainWindow::slotQueItemClicked(QListWidgetItem *it) {
    OpItem *opi = (OpItem *)it;
    curOpName = opi->text();
    curOperator = opi->getOp();
    updateOp();
}

void MainWindow::slotOpItemDelete() {
    if (!opQueWid->selectedItems().isEmpty())
        delete opQueWid->currentItem();
}

void MainWindow::slotAddToQue() {
    if (curOpName == "") {
        curOpName = "U";
    }
    auto *it = new OpItem(curOpName, curOperator);
    opQueWid->insertItem(0, it);
}

void MainWindow::slotApplyOp() {
    if (rtRB->isChecked()) {
        //            curOperator.toZYdec();
        //            const QVector<double> &x = curOperator.findNVec();
        //            //scene->setNewAxis(x[0], x[1], x[2]);   // 4
        //            //scene->rotateN(x[3] * DEG, curOpName); // 4
    } else if (rzyRB->isChecked()) {
        foreach (auto &e, vectors.keys()) {
            e->changeVector(curOperator.applyZYDecomposition(e->getSpike()));
        }
    } else if (rzxRB->isChecked()) {
        foreach (auto &e, vectors.keys()) {
            e->changeVector(curOperator.applyZXDecomposition(e->getSpike()));
        }
    } else if (rxyRB->isChecked()) {
        foreach (auto &e, vectors.keys()) {
            e->changeVector(curOperator.applyXYDecomposition(e->getSpike()));
        }
    }

    appBut->setEnabled(false);
    this->startTimer(50);
}

void MainWindow::slotApplyQue() {
    for (int i = opQueWid->count() - 1; i >= 0; i--) {
        curOperator = ((OpItem *)(opQueWid->item(i)))->getOp();
        curOpName = opQueWid->item(i)->text();
        slotApplyOp();
    }
}

void MainWindow::slotAbout() {
    QMessageBox::about(
        0, "About program",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi volutpat quis nibh mollis "
        "tristique. Duis ac mi id turpis laoreet pretium. Nullam consequat pretium ipsum, sit amet "
        "congue purus. Sed id justo eget velit interdum malesuada. Donec vel ultricies eros. Nam "
        "facilisis elit a augue vestibulum, ac hendrerit nisl vehicula. Proin viverra arcu eget "
        "sapien sagittis congue. Pellentesque accumsan eros eu sem volutpat malesuada. Integer leo "
        "tellus, rutrum porta ullamcorper at, blandit mollis ante. In ex ipsum, accumsan in dictum "
        "eu, pretium luctus purus. Praesent quam quam, mattis ut euismod a, volutpat ut quam. "
        "Etiam faucibus nec lacus eu tempor. Donec eu ligula tempus, aliquam diam a, dignissim "
        "nibh. Sed id lorem sit amet arcu egestas pulvinar.");
}

void MainWindow::fillFieldsOfVector(Spike sp, FIELD exclude) {
    Vector v;
    v.changeVector(sp);

    if (exclude != FIELD::THEPHI) {
        theEd->setText(numberToStr(qRadiansToDegrees(v.the())));
        phiEd->setText(numberToStr(qRadiansToDegrees(v.phi())));
    }

    if (exclude != FIELD::ALPBET) {
        // TODO maybe should create function that converts double to str
        alpEd->setText(numberToStr(v.a().real()));
        reBetEd->setText(numberToStr(v.b().real()) + (v.b().imag() >= 0 ? "+" : "") +
                         numberToStr(v.b().imag()) + "i");
    }

    if (exclude != FIELD::BLOVEC) {
        xEd->setText(numberToStr(v.x()));
        yEd->setText(numberToStr(v.y()));
        zEd->setText(numberToStr(v.z()));
    }
}

void MainWindow::slotReset() {
    removeAllVectors(vectors);
    addVector(new Vector(0., 0.), vectors);
    fillFieldsOfVector(vectors.keys().last()->getSpike());
}

AngInput::AngInput(QWidget *pwgt) : QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
    angEd = new QLineEdit;
    QLabel *angLab = new QLabel("Введите угол в градусах:");

    angLab->setBuddy(angEd);

    QPushButton *bOk = new QPushButton("Ok");
    QPushButton *bCl = new QPushButton("Cancel");
    connect(bOk, SIGNAL(clicked()), SLOT(accept()));
    connect(bCl, SIGNAL(clicked()), SLOT(reject()));

    QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(angLab);
    lay->addWidget(angEd);

    QHBoxLayout *lay1 = new QHBoxLayout;
    lay1->addWidget(bOk);
    lay1->addWidget(bCl);

    lay->addLayout(lay1);
    setLayout(lay);
    setWindowTitle(((QPushButton *)parent())->text());
    setFixedWidth(155);
}
QString AngInput::ang() const { return angEd->text(); }

OpItem::OpItem(QString str, Operator op) : QListWidgetItem(str), oper(op) {
    this->setToolTip(parseComplexToStr(op.a()) + "\t" + parseComplexToStr(op.b()) + "\n" +
                     parseComplexToStr(op.c()) + "\t" + parseComplexToStr(op.d()));
};

Operator OpItem::getOp() { return oper; }
