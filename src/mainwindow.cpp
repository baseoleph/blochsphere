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
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
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
        } else {
            stopTimer();
        }
    }
}

void MainWindow::createSphere() {
    controlWidget = new QWidget(this);
    setCentralWidget(controlWidget);
    controlLayout = new QGridLayout(controlWidget);
    for (int i = 0; i < 1; ++i) {
        for (int j = 1; j < 2; ++j) {
            spheres.append(new Sphere(controlWidget));
            controlLayout->addWidget(spheres.last(), i, j);
        }
    }
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

    //    vectorLabel = new QLabel("Vector:");
    //    vectorPlusBut = new QPushButton("+");
    //    vectorPlusBut->setFixedWidth(50);
    //    vectorMinusBut = new QPushButton("-");
    //    vectorMinusBut->setFixedWidth(50);
    //    vectorMinusBut->setEnabled(false);

    connect(spherePlusBut, SIGNAL(clicked()), SLOT(slotPlusSphere()));
    connect(sphereMinusBut, SIGNAL(clicked()), SLOT(slotMinusSphere()));
    //    connect(vectorPlusBut, SIGNAL(clicked()), SLOT(slotPlusVector()));
    //    connect(vectorMinusBut, SIGNAL(clicked()), SLOT(slotMinusVector()));

    auto vectorSphereCreatorWid = new QWidget();
    auto vectorSphereLay = new QHBoxLayout();
    //    vectorSphereLay->setSpacing(0);
    vectorSphereLay->setMargin(0);
    vectorSphereLay->addWidget(sphereLabel);
    vectorSphereLay->addWidget(spherePlusBut);
    vectorSphereLay->addWidget(sphereMinusBut);
    //    vectorSphereLay->addWidget(vectorLabel);
    //    vectorSphereLay->addWidget(vectorPlusBut);
    //    vectorSphereLay->addWidget(vectorMinusBut);
    vectorSphereCreatorWid->setLayout(vectorSphereLay);

    topTabWid = new QTabWidget();
    topTabWid->setFixedHeight(115);

    // TODO maybe better use function for it
    auto vct = new Vector(0., 0.);
    addVector(vct, vectors, spheres[0]);
    auto vectorWidget = new VectorWidget(topTabWid, vct);

    topTabWid->addTab(vectorWidget, "1");
    mainLay->addWidget(vectorSphereCreatorWid);
    mainLay->addWidget(topTabWid);
    mainLay->addWidget(makeRXYZWid());
    mainLay->addWidget(makeOpWid());
    mainLay->setSpacing(0);
    mainLay->setMargin(0);
    leftWid->setLayout(mainLay);
    leftWid->setFixedWidth(400);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidget(leftWid);

    auto qtb = new QToolBar("Control panel");
    qtb->addWidget(scrollArea);
    qtb->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);

    this->addToolBar(Qt::LeftToolBarArea, qtb);
}

QWidget *MainWindow::makeRXYZWid() {
    rxyzTab = new QTabWidget;
    rxyzTab->insertTab(0, makeRZYWid(), "Z-Y");
    rxyzTab->insertTab(1, makeRZXWid(), "Z-X");
    rxyzTab->insertTab(2, makeRXYWid(), "X-Y");
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

QWidget *MainWindow::makeRZYWid() {
    rZYAlpEd = new QLineEdit("0");
    rZYBetEd = new QLineEdit("0");
    rZYGamEd = new QLineEdit("0");
    rZYDelEd = new QLineEdit("0");

    auto *rZYALab = new QLabel("Alpha");
    auto *rZYBLab = new QLabel("Rz(Beta)");
    auto *rZYGLab = new QLabel("Ry(Gamma)");
    auto *rZYDLab = new QLabel("Rz(Delta)");

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
            connect(mat[i][j], SIGNAL(textEdited(const QString &)),
                    SLOT(slotComplexLineEditChanged(const QString &)));
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
    connect(axOpBut, SIGNAL(clicked()), SLOT(slotSetNewAxOp()));

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

    rzyRb = new QRadioButton("ZY-decomposition");
    rzyRb->toggle();

    rzxRb = new QRadioButton("ZX-decomposition");

    rxyRb = new QRadioButton("XY-decomposition");

    rtRb = new QRadioButton("Rotation about vector");

    connect(rtRb, SIGNAL(toggled(bool)), SLOT(slotToggleRotateVector(bool)));

    auto *gbLay = new QVBoxLayout();
    qGb->setFixedHeight(120);
    gbLay->addWidget(rzyRb);
    gbLay->addWidget(rzxRb);
    gbLay->addWidget(rxyRb);
    gbLay->addWidget(rtRb);
    gbLay->setSpacing(5);
    qGb->setLayout(gbLay);

    appBut = new QPushButton("Apply operator");
    appBut->setFixedHeight(35);
    connect(appBut, SIGNAL(clicked()), SLOT(slotApplyOp()));

    auto *addToQueBut = new QPushButton("Add to queue");
    addToQueBut->setFixedHeight(35);
    connect(addToQueBut, SIGNAL(clicked()), SLOT(slotAddToQue()));

    currentOperatorLabel = new QLabel();
    updateCurOperatorTable();

    auto *horLay = new QHBoxLayout;
    horLay->addWidget(appBut);
    horLay->addWidget(addToQueBut);

    auto *pLay = new QVBoxLayout();
    pLay->addWidget(stackW);
    pLay->addWidget(qGb);
    pLay->addWidget(currentOperatorLabel);
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
    curOpName = str.c_str();
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
}

void MainWindow::slotSetRandomOp() {
    curOpName = "U";
    curOperator.setOperator(Operator::genRandUnitaryMatrix(time(nullptr)));

    // TODO may change matrix fields
    updateOp();
}

void MainWindow::slotSetNewAxOp() {
    QRegExp rxp("^(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?[\\d]*e?[+-]?[\\d]*);(-?[\\d]*\\.?"
                "[\\d]*e?[+-]?[\\d]*)$");
    if (axRnEd->text().contains(rxp)) {
        vectorangle va;
        va.angle = ngRnEd->text().toDouble() * M_PI / 180;
        va.x = rxp.capturedTexts()[1].toDouble();
        va.y = rxp.capturedTexts()[2].toDouble();
        va.z = rxp.capturedTexts()[3].toDouble();

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

        curOpName = "U";
        updateOp();
    } else {
        QMessageBox::warning(this, "Error", "Wrong input: Vector (x;y;z)");
        return;
    }
}

void MainWindow::updateOp() {
    decomposition zyDec = curOperator.zyDecomposition();
    rZYAlpEd->setText(numberToStr(zyDec.alpha));
    rZYBetEd->setText(numberToStr(zyDec.beta));
    rZYGamEd->setText(numberToStr(zyDec.gamma));
    rZYDelEd->setText(numberToStr(zyDec.delta));

    decomposition zxDec = curOperator.zxDecomposition();
    rZXAlpEd->setText(numberToStr(zxDec.alpha));
    rZXBetEd->setText(numberToStr(zxDec.beta));
    rZXGamEd->setText(numberToStr(zxDec.gamma));
    rZXDelEd->setText(numberToStr(zxDec.delta));

    decomposition xyDec = curOperator.xyDecomposition();
    rXYAlpEd->setText(numberToStr(xyDec.alpha));
    rXYBetEd->setText(numberToStr(xyDec.beta));
    rXYGamEd->setText(numberToStr(xyDec.gamma));
    rXYDelEd->setText(numberToStr(xyDec.delta));

    mat[0][0]->setText(parseComplexToStr(curOperator.getOperator().a()));
    mat[0][1]->setText(parseComplexToStr(curOperator.getOperator().b()));
    mat[1][0]->setText(parseComplexToStr(curOperator.getOperator().c()));
    mat[1][1]->setText(parseComplexToStr(curOperator.getOperator().d()));

    vectorangle va = curOperator.vectorAngleDec();
    axRnEd->setText(
        QString("%1;%2;%3").arg(roundNumber(va.x)).arg(roundNumber(va.y)).arg(roundNumber(va.z)));
    ngRnEd->setText(QString("%1").arg(roundNumber(va.angle * 180 / M_PI)));

    foreach (auto e, spheres) { e->setRotateVector(QVector3D(va.x, va.y, va.z)); }
    updateCurOperatorTable();
}

// TODO check
void MainWindow::slotQueItemClicked(QListWidgetItem *it) {
    auto *opi = (OpItem *)it;
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
    stopTimer();
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
    appBut->setEnabled(false);
    appQueBut->setEnabled(false);
    if (not isQueueAnimation) {
        startTimer();
    }
}

CurDecompFun MainWindow::getCurrentDecomposition() {
    if (rtRb->isChecked()) {
        return &Operator::applyVectorRotation;
    } else if (rzyRb->isChecked()) {
        return &Operator::applyZYDecomposition;
    } else if (rzxRb->isChecked()) {
        return &Operator::applyZXDecomposition;
    } else if (rxyRb->isChecked()) {
        return &Operator::applyXYDecomposition;
    }
}

void MainWindow::slotAbout() {
    QMessageBox::about(
        this, "About program",
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
void MainWindow::updateCurOperatorTable() {
    currentOperatorLabel->setText("<table>"
                                  "<tr>"
                                  "<td>" +
                                  curOperator.getOperator().aStr() +
                                  "</td>"
                                  "<td>" +
                                  curOperator.getOperator().bStr() +
                                  "</td>"
                                  "</tr><tr>"
                                  "<td>" +
                                  curOperator.getOperator().cStr() +
                                  "</td>"
                                  "<td>" +
                                  curOperator.getOperator().dStr() +
                                  "</td>"

                                  "</tr></table>");
}
void MainWindow::slotToggleRotateVector(bool f) {
    foreach (auto e, spheres) { e->setEnabledRotateVector(f); }
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
        controlLayout->addWidget(spheres.last(), 0, spheres.size());

        auto vct = new Vector(0., 0.);
        addVector(vct, vectors, spheres.last());
        auto vectorWidget = new VectorWidget(topTabWid, vct);
        topTabWid->addTab(vectorWidget, QString::number(spheres.size()));
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
    }

    spherePlusBut->setEnabled(spheres.size() < MAX_COUNT_SPHERES);
    sphereMinusBut->setEnabled(spheres.size() > 1);
}
void MainWindow::startTimer() {
    if (not tm->isActive()) {
        statusBar()->showMessage("start");
        qDebug() << "start";
        tm->start(50);
    }
    setEnabledWidgets(false);
}
void MainWindow::stopTimer() {
    tm->stop();
    statusBar()->showMessage("stop");
    qDebug() << "stop";
    setEnabledWidgets(true);
}
void MainWindow::setEnabledWidgets(bool f) {
    appBut->setEnabled(f);
    appQueBut->setEnabled(f);
    spherePlusBut->setEnabled(f and spheres.size() < MAX_COUNT_SPHERES);
    sphereMinusBut->setEnabled(f and spheres.size() > 1);
}
