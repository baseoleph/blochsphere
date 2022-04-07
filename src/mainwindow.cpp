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
    addVector();
}

void MainWindow::addVector() {
    auto *v = new Vector(qDegreesToRadians(0.), qDegreesToRadians(0.));
    vectors.insert(v, QVector<Sphere *>());

    for (auto &e : spheres) {
        e->addVector(v);
        vectors[v].append(e);
    }
}

void MainWindow::removeVector(Vector *v) {
    for (auto &e : vectors[v]) {
        e->deleteVector(v);
        e->update();
    }
    vectors.remove(v);
    v->~Vector();
}

void MainWindow::removeAllVectors() {
    while (not vectors.isEmpty()) {
        removeVector(vectors.lastKey());
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

void MainWindow::createOldScene() {
    controlWidget = new QWidget(this);

    QWidget *w = new QWidget(this);
    setCentralWidget(w);
    QGridLayout *layout = new QGridLayout(w);
    w->setLayout(layout);
    for (int i = 0; i < 1; ++i) {
        for (int j = 1; j < 2; ++j) {
            spheres.append(new Sphere(w, QString::number(i + 10 * j)));
            layout->addWidget(spheres.last(), i, j);
        }
    }
    layout->addWidget(controlWidget, 0, 0);
}

void MainWindow::setupOldControlBlock() {
    controlWidget->setMaximumWidth(200);
    QGridLayout *layout = new QGridLayout(controlWidget);
    controlWidget->setLayout(layout);
    QPushButton *bAddVector = new QPushButton("Add vector", this);
    QPushButton *bResetVectors = new QPushButton("Reset vector", this);
    QPushButton *bRotateVectors = new QPushButton("Rotate vector", this);
    layout->addWidget(bAddVector, 0, 0);
    layout->addWidget(bResetVectors, 0, 1);
    layout->addWidget(bRotateVectors, 0, 2);
    bAddVector->show();
    bResetVectors->show();
    bRotateVectors->show();
    connect(bAddVector, &QPushButton::clicked, this, &MainWindow::addVector);
    connect(bResetVectors, &QPushButton::clicked, this, &MainWindow::removeAllVectors);
}

//////
///-----------------------------------------------------------------------------------------------

// TODO move
double okr(double a, double s) {
    a *= s;
    if (a - floor(a) >= 0.5)
        return (floor(a) + 1) / s;
    else
        return floor(a) / s;
}

void MainWindow::createSphere() {

    // TODO get rid of
    // scene = new Bloch();
    // scene->setFocus();
    //        this->setCentralWidget(//scene);
    //        connect(//scene, SIGNAL(motionBegin(QString)), SLOT(slotMotionBegin(QString)));
    //        connect(//scene, SIGNAL(motionEnd()), SLOT(slotMotionEnd()));

    //        QBVector *qV = //scene->getQBV();
    //        connect(qV, SIGNAL(phiTheChanged(float, float)), SLOT(slotPhiTheChanged(float,
    //        float))); connect(qV, SIGNAL(alpBetChanged(float, complex)),
    //        SLOT(slotAlpBetChanged(float, complex))); connect(qV, SIGNAL(xyzChanged(float, float,
    //        float)), SLOT(slotXYZChanged(float, float, float))); connect(qV,
    //        SIGNAL(newOp(Operator &)), SLOT(slotNewOp(Operator &))); qV->report();
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

void MainWindow::createActions() {
    aboutAct = new QAction("About program", this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::slotAbout);

    //        saveState = new QAction("Save state", this);
    //        connect(saveState, SIGNAL(triggered()), SLOT(slotSaveState()));
    //
    //        recallState = new QAction("Recall state", this);
    //        recallState->setEnabled(false);
    //        connect(recallState, SIGNAL(triggered()), SLOT(slotRecallState()));
    //
    //        resetAct = new QAction("Reset", this);
    //        connect(resetAct, SIGNAL(triggered()), SLOT(slotReset()));
    //
    //     QAction *applyAct;
    //     QAction *addToQueAct;
    //
    //        drawTAct = new QAction("Draw trace", this);
    //        drawTAct->setCheckable(true);
    //        drawTAct->setChecked(true);
    //        connect(drawTAct, SIGNAL(triggered()), SLOT(slotDrawTrace()));
    //
    //        clearTAct = new QAction("Clear trace", this);
    //        connect(clearTAct, SIGNAL(triggered()), SLOT(slotClearTrace()));
    //
    //        trColor = new QActionGroup(this);
    //        trColor->addAction("Red")->setChecked(true);
    //        trColor->addAction("Green");
    //        trColor->addAction("Blue");
    //        trColor->addAction("Grey");
    //        trColor->addAction("Yellow");
    //        foreach (QAction *act, trColor->actions())
    //            act->setCheckable(true);
    //        connect(trColor, SIGNAL(triggered(QAction *)), SLOT(slotTraceColor(QAction *)));
    //
    //     QActionGroup *rotMode;

    exitAct = new QAction("Exit", this);
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::createMenu() {
    auto *mnuBar = new QMenuBar();
    auto *menuFile = new QMenu("File");
    auto *menuInfo = new QMenu("Info");

    menuFile->addSeparator();
    menuFile->addAction(exitAct);
    menuInfo->addAction(aboutAct);

    // QMenu  *menuColor = new QMenu("Color");
    // menuColor->addActions(trColor->actions());

    mnuBar->addMenu(menuFile);
    mnuBar->addMenu(menuInfo);

    this->setMenuBar(mnuBar);
}

void MainWindow::createTopBar() {
    auto *qtb = new QToolBar("Tool bar");
    //    qtb->addAction(saveState);
    //    qtb->addAction(recallState);
    //    qtb->addAction(resetAct);
    qtb->addSeparator();
    //    qtb->addAction(drawTAct);
    //    qtb->addAction(clearTAct);

    qcomb = new QComboBox(qtb);
    //    foreach (QAction *act, trColor->actions())
    //        qcomb->addItem(act->text());
    connect(qcomb, SIGNAL(currentIndexChanged(int)), SLOT(slotTraceColor(int)));
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
    connect(thePhiButton, &QPushButton::clicked, this, &MainWindow::slotThePhi);

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
    connect(bPsi, &QPushButton::clicked, this, &MainWindow::slotAlpBet);

    auto *bRandPsi = new QPushButton("Random");
    bRandPsi->setFixedWidth(60);
    connect(bRandPsi, &QPushButton::clicked, this, &MainWindow::slotSetRandomPsi);

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
    connect(bPsi, &QPushButton::clicked, this, &MainWindow::slotBloVec);

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

    QPushButton *bRotXYZ = new QPushButton("Set");
    bRotXYZ->setFixedWidth(60);
    connect(bRotXYZ, SIGNAL(clicked()), SLOT(slotSetRXYZOp()));

    QVBoxLayout *qwb = new QVBoxLayout;
    qwb->addWidget(rxyzTab);
    // qwb->addWidget(bRotXYZ);
    qwb->setSpacing(0);
    qwb->setMargin(1);
    QWidget *rtW = new QWidget;
    rtW->setLayout(qwb);
    rtW->setFixedHeight(140);

    return rtW;
}

QWidget *MainWindow::makeRZYWid() {
    rZYAlpEd = new QLineEdit();
    rZYBetEd = new QLineEdit();
    rZYGamEd = new QLineEdit();
    rZYDelEd = new QLineEdit();
    QString a;

    QLabel *rZYALab = new QLabel("Alpha");
    QLabel *rZYBLab = new QLabel("Rz(Beta)");
    QLabel *rZYGLab = new QLabel("Ry(Gamma)");
    QLabel *rZYDLab = new QLabel("Ry(Delta)");

    rZYAlpEd->setMaximumWidth(60);
    rZYAlpEd->setReadOnly(true);

    rZYBetEd->setMaximumWidth(60);
    rZYBetEd->setReadOnly(true);

    rZYGamEd->setMaximumWidth(60);
    rZYGamEd->setReadOnly(true);

    rZYDelEd->setMaximumWidth(60);
    rZYDelEd->setReadOnly(true);

    QWidget *rzW = new QWidget();

    QGridLayout *rzLay = new QGridLayout();
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
    rZXAlpEd = new QLineEdit();
    rZXBetEd = new QLineEdit();
    rZXGamEd = new QLineEdit();
    rZXDelEd = new QLineEdit();
    QString a;

    QLabel *rZXALab = new QLabel("Alpha");
    QLabel *rZXBLab = new QLabel("Rz(Beta)");
    QLabel *rZXGLab = new QLabel("Rx(Gamma)");
    QLabel *rZXDLab = new QLabel("Rz(Delta)");

    rZXAlpEd->setMaximumWidth(60);
    rZXAlpEd->setReadOnly(true);

    rZXBetEd->setMaximumWidth(60);
    rZXBetEd->setReadOnly(true);

    rZXGamEd->setMaximumWidth(60);
    rZXGamEd->setReadOnly(true);

    rZXDelEd->setMaximumWidth(60);
    rZXDelEd->setReadOnly(true);

    QWidget *rzW = new QWidget();

    QGridLayout *rzLay = new QGridLayout();
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
    rXYAlpEd = new QLineEdit();
    rXYBetEd = new QLineEdit();
    rXYGamEd = new QLineEdit();
    rXYDelEd = new QLineEdit();
    QString a;

    QLabel *rXYALab = new QLabel("Alpha");
    QLabel *rXYBLab = new QLabel("Rx(Beta)");
    QLabel *rXYGLab = new QLabel("Ry(Gamma)");
    QLabel *rXYDLab = new QLabel("Rx(Delta)");

    rXYAlpEd->setMaximumWidth(60);
    rXYAlpEd->setReadOnly(true);

    rXYBetEd->setMaximumWidth(60);
    rXYBetEd->setReadOnly(true);

    rXYGamEd->setMaximumWidth(60);
    rXYGamEd->setReadOnly(true);

    rXYDelEd->setMaximumWidth(60);
    rXYDelEd->setReadOnly(true);

    QWidget *rzW = new QWidget();

    QGridLayout *rzLay = new QGridLayout();
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

QWidget *MainWindow::makeOpWid() {
    QWidget *opW = new QWidget();

    QFrame *qfOpButtons = new QFrame(opW);
    qfOpButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
    qfOpButtons->move(0, 0);
    qfOpButtons->setFixedSize(opW->size());

    stackW = new QTabWidget();
    stackW->setFixedHeight(120);

    QWidget     *bOp = new QWidget();
    QGridLayout *bOpLay = new QGridLayout();
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

    QWidget     *mOp = new QWidget();
    QGridLayout *mOpLay = new QGridLayout();

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            mat[i][j] = new QLineEdit();
            mat[i][j]->setValidator(&compValid);
        }

    QPushButton *applyMat = new QPushButton("Ok");
    connect(applyMat, SIGNAL(clicked()), SLOT(slotSetMatrixOp()));

    mOpLay->addWidget(mat[0][0], 1, 0);
    mOpLay->addWidget(mat[0][1], 1, 5);
    mOpLay->addWidget(mat[1][0], 2, 0);
    mOpLay->addWidget(mat[1][1], 2, 5);
    mOpLay->addWidget(applyMat, 3, 5);
    mOpLay->setContentsMargins(10, 10, 10, 0);
    mOp->setLayout(mOpLay);

    stackW->addTab(mOp, "Matrix");

    QWidget *rNw = new QWidget();
    axRnEd = new QLineEdit();
    axRnEd->setValidator(&axisValid);
    ngRnEd = new QLineEdit();
    ngRnEd->setValidator(new QDoubleValidator);

    QPushButton *axOpBut = new QPushButton("Ok");
    connect(axOpBut, SIGNAL(clicked()), SLOT(slotSetNewAxOp()));

    QGridLayout *rNwLay = new QGridLayout();
    rNwLay->addWidget(new QLabel("Vector"), 0, 0);
    rNwLay->addWidget(axRnEd, 0, 1, 1, 3);
    rNwLay->addWidget(new QLabel("Angle"), 1, 0);
    rNwLay->addWidget(ngRnEd, 1, 1, 1, 3);
    rNwLay->addWidget(axOpBut, 2, 3, 1, 1); // 4
    rNwLay->setContentsMargins(20, 10, 20, 0);
    rNwLay->setSpacing(5);
    rNw->setLayout(rNwLay);

    stackW->addTab(rNw, "Rn");
    // ----------------------

    // Выбор режима вращения
    // ---------------------
    QGroupBox *qGb = new QGroupBox("Rotation");

    rzyRB = new QRadioButton("ZY-decomposition");
    rzyRB->toggle();
    connect(rzyRB, SIGNAL(clicked()), SLOT(slotHideNewAx()));

    rzxRB = new QRadioButton("ZX-decomposition");
    connect(rzyRB, SIGNAL(clicked()), SLOT(slotHideNewAx()));

    rxyRB = new QRadioButton("XY-decomposition");
    connect(rzyRB, SIGNAL(clicked()), SLOT(slotHideNewAx()));

    rtRB = new QRadioButton("Rotation about vector");

    QVBoxLayout *gbLay = new QVBoxLayout();
    qGb->setFixedHeight(120);
    gbLay->addWidget(rzyRB);
    gbLay->addWidget(rzxRB);
    gbLay->addWidget(rxyRB);
    gbLay->addWidget(rtRB);
    gbLay->setSpacing(5);
    qGb->setLayout(gbLay);

    appBut = new QPushButton("Apply operator");
    appBut->setFixedHeight(35);
    connect(appBut, SIGNAL(clicked()), SLOT(slotApplyOp()));
    QPushButton *addToQueBut = new QPushButton("Add to queue");
    addToQueBut->setFixedHeight(35);
    connect(addToQueBut, SIGNAL(clicked()), SLOT(slotAddToQue()));

    QHBoxLayout *horLay = new QHBoxLayout;
    horLay->addWidget(appBut);
    horLay->addWidget(addToQueBut);

    QVBoxLayout *pLay = new QVBoxLayout();
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
    QPushButton *newBut = new QPushButton(str);
    newBut->setFixedHeight(26);
    connect(newBut, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    return newBut;
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
    double the = QRandomGenerator::global()->bounded(180.);
    double phi = QRandomGenerator::global()->bounded(360.);

    Spike sp = Vector::createSpike(the, phi);
    foreach (auto &e, vectors.keys()) { e->changeVector(sp); }

    fillFieldsOfVector(sp);
}

void MainWindow::slotPhiTheChanged(float phi, float the) {
    QString t = QString("%1").arg(okr(the * DEG, 10)), p = QString("%1").arg(okr(phi * DEG, 10));
    theEd->setText(t);
    phiEd->setText(p);
}

void MainWindow::slotAlpBetChanged(float a, complex b) {
    QString alp = QString("%1").arg(okr(a, 1000.0f)), bet = deparceC(b, 1000);
    alpEd->setText(alp);
    reBetEd->setText(bet);
}

void MainWindow::slotXYZChanged(float x, float y, float z) { // 4
    xyzStLab->setText(QString("<font size=4>(%1;%2;%3)</font>")
                          .arg(okr(x, 100))
                          .arg(okr(y, 100))
                          .arg(okr(z, 100)));
}

void MainWindow::slotSaveState() {
    //    savedAlp = //scene->getQBV()->_a();
    //    savedBet = //scene->getQBV()->_b();
    //    QString str = QString("<font size=4>Saved state: (%1, ").arg(okr(savedAlp, 100));
    //    str += deparceC(savedBet, 100) + ")</font>";
    //    svdStLab->setText(str);
    //    svdStLab->show();
    //    if (!recallState->isEnabled())
    //        recallState->setEnabled(true);
}

void MainWindow::slotRecallState() {
    // scene->setAB(savedAlp, savedBet);
    // scene->getQBV()->report();
}

void MainWindow::slotMotionBegin(QString msg) {
    mtnStLab->setText("<font size=4>" + msg + "</font>");
    mtnStLab->setVisible(true);
}

void MainWindow::slotMotionEnd() { mtnStLab->setVisible(false); }

void MainWindow::slotDrawTrace() { /*//scene->setDrawTrace(drawTAct->isChecked());*/
}

void MainWindow::slotClearTrace() { /* scene->clearTrace();*/
}

void MainWindow::slotTraceColor(int n) {
    //    if (trColor->checkedAction() != trColor->actions()[n])
    //        trColor->actions()[n]->setChecked(true);

    // scene->setTraceColor(n);
}

void MainWindow::slotTraceColor(QAction *act) {
    //    int i = trColor->actions().indexOf(act);
    //    if (qcomb->currentIndex() != i)
    //        qcomb->setCurrentIndex(i);
}

void MainWindow::slotButtonClicked() {
    std::string str = ((QPushButton *)sender())->text().toStdString();
    Operator    op;
    AngInput   *aIn;
    QString     axSt = axRnEd->text();
    bool        rzyCond = rzyRB->isChecked();

    //        switch (str[0]) {
    //        case 'X':
    //            op = Xop;
    //            break;
    //        case 'Y':
    //            op = Yop;
    //            break;
    //        case 'Z':
    //            op = Zop;
    //            break;
    //        case 'H':
    //            op = Hop;
    //            break;
    //        case 'S':
    //            op = Sop;
    //            break;
    //        case 'T':
    //            op = Top;
    //            break;
    //        case 'P':
    //            aIn = new AngInput((QWidget *)sender());
    //            if (aIn->exec() == QDialog::Accepted)
    //                op = PHIop(aIn->ang().toFloat() * RAD);
    //            else {
    //                delete aIn;
    //                return;
    //            }
    //            delete aIn;
    //            break;
    //        case 'R':
    //            aIn = new AngInput((QWidget *)sender());
    //            if (aIn->exec() == QDialog::Accepted) {
    //                float the = (aIn->ang().toFloat()) * RAD;
    //                if (str[1] == 'x')
    //                    op = RXop(the);
    //                else if (str[1] == 'y')
    //                    op = RYop(the);
    //                else if (str[1] == 'z')
    //                    op = RZop(the);
    //
    //            } else {
    //                delete aIn;
    //                return;
    //            }
    //            delete aIn;
    //            break;
    //        }
    curOperator = op;
    curOpName = str.c_str();
    updateOp();
}

void MainWindow::slotNewOp(Operator &op) {
    curOperator = op;
    updateOp();
}

void MainWindow::slotSetRXYZOp() {
    double alpha, beta, gamma, delta, v;
    switch (rxyzTab->currentIndex()) {
    case 0:
        alpha = rZYAlpEd->text().toDouble() * RAD;
        beta = rZYBetEd->text().toDouble() * RAD;
        gamma = rZYAlpEd->text().toDouble() * RAD;
        delta = rZYDelEd->text().toDouble() * RAD;
        v = gamma / 2.0;
        //            curOperator = Operator(exp(C_I * (alpha - beta / 2.0 - delta / 2.0)) *
        //            cos(v),
        //                                     exp(C_I * (alpha - beta / 2.0 + delta / 2.0)) *
        //                                     sin(v), exp(C_I * (alpha + beta / 2.0 - delta / 2.0))
        //                                     * sin(v), exp(C_I * (alpha + beta / 2.0 + delta
        //                                     / 2.0)) * cos(v));
        break;
    case 1:
        alpha = rZXAlpEd->text().toDouble() * RAD;
        beta = rZXBetEd->text().toDouble() * RAD;
        gamma = rZXAlpEd->text().toDouble() * RAD;
        delta = rZXDelEd->text().toDouble() * RAD;
        v = gamma / 2.0;
        //            curOperator =
        //                Operator(exp(C_I * (alpha - beta / 2.0 - delta / 2.0)) * cos(v),
        //                           -C_I * exp(C_I * (alpha - beta / 2.0 + delta / 2.0)) * sin(v),
        //                           -C_I * exp(C_I * (alpha + beta / 2.0 - delta / 2.0)) * sin(v),
        //                           exp(C_I * (alpha + beta / 2.0 + delta / 2.0)) * cos(v));
        break;
    case 2:
        alpha = rXYAlpEd->text().toDouble() * RAD;
        beta = rXYBetEd->text().toDouble() * RAD;
        gamma = rXYAlpEd->text().toDouble() * RAD;
        delta = rXYDelEd->text().toDouble() * RAD;
        v = gamma / 2.0;
        double w = beta / 2.0 + delta / 2.0, u = beta / 2.0 - delta / 2.0;
        //            curOperator =
        //                Operator(exp(C_I * alpha) * (cos(v) * cos(w) - C_I * sin(v) * sin(u)),
        //                           exp(C_I * alpha) * (-sin(v) * cos(u) - C_I * cos(v) * sin(w)),
        //                           exp(C_I * alpha) * (sin(v) * cos(u) - C_I * cos(v) * sin(w)),
        //                           exp(C_I * alpha) * (cos(v) * cos(w) + C_I * sin(v) * sin(u)));
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
    //        Operator op(res[0], res[1], res[2], res[3]); // 4

    //        if (!op.isUnitary()) {
    //            QMessageBox::warning(0, "Error", "Matrix must be unitary");
    //            return;
    //        }
    //
    //        curOpName = "U";
    //        slotNewOp(op);
}

void MainWindow::slotSetNewAxOp() {
    QRegExp rxp("^(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?"
                "[\\d]*e?[+-]?[\\d]*)$");
    if (axRnEd->text().contains(rxp)) {
        double ng = ngRnEd->text().toDouble() * RAD, nX = rxp.capturedTexts()[1].toDouble(),
               nY = rxp.capturedTexts()[2].toDouble(), nZ = rxp.capturedTexts()[3].toDouble();
        // 4
        double k = (nX < 0) ? -1.0 : 1.0, l = (nY < 0) ? -1.0 : 1.0, m = (nZ < 0) ? -1.0 : 1.0;
        double n = nX * nX + nY * nY + nZ * nZ;

        nX = k * sqrt(nX * nX / n);
        nY = l * sqrt(nY * nY / n);
        nZ = m * sqrt(nZ * nZ / n);

        // scene->setNewAxis(nX, nY, nZ);

        //        curOperator = cos(ng / 2.0) * Iop - C_I * sin(ng / 2.0) * (nX * Xop + nY * Yop +
        //        nZ * Zop);

        curOpName = "U";
        updateOp();
    } else {
        QMessageBox::warning(0, "Error", "Wrong input: Vector (x;y;z)");
        return;
    }
}

void MainWindow::updateOp() {
    //        curOperator.toZXdec();
    //        rZXAlpEd->setText(QString("%1").arg(okr(curOperator._alp() * DEG, 10)));
    //        rZXBetEd->setText(QString("%1").arg(okr(curOperator._bet() * DEG, 10)));
    //        rZXGamEd->setText(QString("%1").arg(okr(curOperator._gam() * DEG, 10)));
    //        rZXDelEd->setText(QString("%1").arg(okr(curOperator._del() * DEG, 10)));
    //
    //        curOperator.toXYdec();
    //        rXYAlpEd->setText(QString("%1").arg(okr(curOperator._alp() * DEG, 10)));
    //        rXYBetEd->setText(QString("%1").arg(okr(curOperator._bet() * DEG, 10)));
    //        rXYGamEd->setText(QString("%1").arg(okr(curOperator._gam() * DEG, 10)));
    //        rXYDelEd->setText(QString("%1").arg(okr(curOperator._del() * DEG, 10)));
    //
    //        curOperator.toZYdec();
    //        rZYAlpEd->setText(QString("%1").arg(okr(curOperator._alp() * DEG, 10)));
    //        rZYBetEd->setText(QString("%1").arg(okr(curOperator._bet() * DEG, 10)));
    //        rZYGamEd->setText(QString("%1").arg(okr(curOperator._gam() * DEG, 10)));
    //        rZYDelEd->setText(QString("%1").arg(okr(curOperator._del() * DEG, 10)));
    //
    //        mat[0][0]->setText(deparceC(curOperator._a()));
    //        mat[0][1]->setText(deparceC(curOperator._b()));
    //        mat[1][0]->setText(deparceC(curOperator._c()));
    //        mat[1][1]->setText(deparceC(curOperator._d()));
    //
    //        QVector<double> x = curOperator.findNVec();
    //        axRnEd->setText(QString("%1;%2;%3")
    //                            .arg(okr(x[0], 1000))
    //                            .arg(okr(x[1], 1000))
    //                            .arg(okr(x[2], 1000)));          // 4
    //        ngRnEd->setText(QString("%1").arg(okr(x[3] * DEG))); // 4
}

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
    if (curOpName == "")
        return;
    OpItem *it = new OpItem(curOpName, curOperator);
    opQueWid->insertItem(0, it);
}

void MainWindow::slotApplyOp() {
    if (rtRB->isChecked()) {
        statusBar()->showMessage("rb");
        //            curOperator.toZYdec();
        //            const QVector<double> &x = curOperator.findNVec();
        //            //scene->setNewAxis(x[0], x[1], x[2]);   // 4
        //            //scene->rotateN(x[3] * DEG, curOpName); // 4
    } else if (rzyRB->isChecked()) {
        statusBar()->showMessage("rzy");
        //            curOperator.toZYdec();
        //            //scene->rotateZY(curOperator._del() * DEG, curOperator._gam() * DEG,
        //                            curOperator._bet() * DEG, curOpName);
    } else if (rzxRB->isChecked()) {
        statusBar()->showMessage("rzx");
        foreach (auto &e, vectors.keys()) {
            UnitaryMatrix2x2 newOp;
            newOp.updateMatrix(0, 1, 1, 0);
            curOperator.setOperator(newOp);
            e->changeVector(curOperator.applyOperator(e->getSpike()));
        }

    } else if (rxyRB->isChecked()) {
        statusBar()->showMessage("rxy");
        //            curOperator.toXYdec();
        //            //scene->rotateXY(curOperator._del() * DEG, curOperator._gam() * DEG,
        //                            curOperator._bet() * DEG, curOpName);
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
        theEd->setText(QString::number(qRadiansToDegrees(v.the())));
        phiEd->setText(QString::number(qRadiansToDegrees(v.phi())));
    }

    if (exclude != FIELD::ALPBET) {
        // TODO maybe should create function that converts double to str
        alpEd->setText(QString::number(v.a().real(), 'f', 3));
        reBetEd->setText(QString::number(v.b().real(), 'f', 3) + (v.b().imag() >= 0 ? "+" : "") +
                         QString::number(v.b().imag(), 'f', 3) + "i");
    }

    if (exclude != FIELD::BLOVEC){
        xEd->setText(QString::number(v.x()));
        yEd->setText(QString::number(v.y()));
        zEd->setText(QString::number(v.z()));
    }
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

OpItem::OpItem(QString str, Operator op)
    : QListWidgetItem(str),
      oper(op){
          //        this->setToolTip(deparceC(op._a()) + "\t" + deparceC(op._b()) + "\n" +
          //                         deparceC(op._c()) + "\t" + deparceC(op._d()));
      };

Operator OpItem::getOp() { return oper; }

complex parseStrToComplex(const QString &str) {
    QRegExp rxp1("^([+-]?[0-9]+\\.?[0-9]*)([+-]?[0-9]*\\.?[0-9]*)i$");
    QRegExp rxp2("^([+-]?[\\d]+\\.?[\\d]*)$");
    QRegExp rxp3("^([+-]?[0-9]*\\.?[0-9]*)i$");
    QRegExp rxp4("^([+-])i$");
    if (str.contains(rxp1)) {
        if (rxp1.capturedTexts()[2] == "+")
            return complex(rxp1.capturedTexts()[1].toDouble(), 1.0);
        else if (rxp1.capturedTexts()[2] == "-")
            return complex(rxp1.capturedTexts()[1].toDouble(), -1.0);
        else
            return complex(rxp1.capturedTexts()[1].toDouble(), rxp1.capturedTexts()[2].toDouble());
    } else if (str.contains(rxp2))
        return complex(rxp2.capturedTexts()[1].toDouble(), 0.0);
    else if (str.contains(rxp3)) {
        if (rxp3.capturedTexts()[1] == "+" || rxp3.capturedTexts()[1] == "")
            return complex(0.0, 1.0);
        else if (rxp3.capturedTexts()[1] == "-")
            return complex(0.0, -1.0);
        else
            return complex(0.0, rxp3.capturedTexts()[1].toDouble());
    }
    // DOTO nothing try to catch it
    throw 1;
}

QString deparceC(complex c, int d) {
    QString str;
    float   im = okr(imag(c), d), re = okr(real(c), d);
    if (abs(re) < EPS && abs(im) > EPS) {
        if (abs(1.0 - im) < EPS)
            return str += "i";
        else if (abs(1.0 + im) < EPS)
            return str += "-i";
        else
            return str += QString("%1i").arg(im);
    } else {
        str = QString("%1").arg(re);
        if (abs(im) > EPS) {
            if (im > EPS) {
                str += "+";
                if (1.0 - im < EPS)
                    return str += "i";
            }
            if (im < EPS) {
                if (1.0 + im < EPS)
                    return str += "-i";
            }
            str += QString("%1i").arg(im);
        }
        return str;
    }
    // DOTO nothing try to catch it
    throw 1;
}

QString decompString(double a, double b, double g, double d) {
    QString res = "<font size=4><center>U=";
    if (abs(a) > 0.001) {
        res += "e<sup><font face=symbol size=5>i";
        if (abs(1 - a) > 0.001)
            res += QString("%1").arg(a);
        res += "p</font></sup>";
    }
    if (abs(b) > 0.001) {
        res += "R<sub><font size=5>z</font></sub>(<font face=symbol>";
        if (abs(1 - b) > 0.001)
            res += QString("%1").arg(b);
        res += "p</font>)";
    }
    if (abs(g) > 0.001) {
        res += "R<sub><font size=5>y</font></sub>(<font face=symbol>";
        if (abs(1 - g) > 0.001)
            res += QString("%1").arg(g);
        res += "p</font>)";
    }
    if (abs(d) > 0.001) {
        res += "R<sub><font size=5>z</font></sub>(<font face=symbol>";
        if (abs(1 - d) > 0.001)
            res += QString("%1").arg(d);
        res += "p</font>)";
    }
    return res += "</center></font>";
}
