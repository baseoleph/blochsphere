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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "Circuit.h"
#include "VectorWidget.h"
#include "blochUtility.h"
#include "operator.hpp"
#include "opitem.hpp"
#include "qubit.hpp"
#include "sphere.hpp"
#include <QActionGroup>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QRadioButton>
#include <QVector>

enum OPERATOR_FORM { NOTHING = 0, MATRIX, VECTOR };

typedef QMap<Vector *, QVector<Sphere *>> MapVectors;

typedef QVector<Spike> (Operator::*CurDecompFun)(Spike);

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void signalAnimating(bool f);

public slots:
    void        addVector(Vector *v, MapVectors &mp);
    static void addVector(Vector *v, MapVectors &mp, Sphere *sph);
    static void removeVector(Vector *v, MapVectors &mp);
    static void removeAllVectors(MapVectors &mp);

    void slotSetOperatorClicked();
    void slotSetMatrixOp();
    void slotSetRandomOp();
    void slotSetRXYZOp();
    void slotSetAxOp();
    void slotQueItemClicked(QListWidgetItem *it);
    void slotOpItemDelete();

    void slotApplyOp();
    void slotAddToQue();
    void slotApplyQue();

    void slotUpdateSpheres();
    void slotReset();
    void slotSaveState();
    void slotRecallState();
    void slotShowTrace();
    void slotClearTrace();
    void slotTraceColor(int index);
    void slotSpeedUp();
    void slotSpeedDown();
    void slotToNormal();
    void slotToYoZ();
    void slotToXoY();
    void slotToZoX();

    void slotComplexLineEditChanged(const QString &);
    void slotToggleRotateVector(bool f);
    void slotToggleAutoNormalize(bool f);
    void slotAbout();

    void slotPlusSphere();
    void slotMinusSphere();

    void slotTimer();

    void slotStartCircuitMove();

private:
    QVector<Sphere *> spheres;
    MapVectors        vectors;
    MapVectors        savedVectors;
    bool              isAutoNormalize = true;
    QTimer           *tm = nullptr;
    Circuit          *circuit = nullptr;

    int circuitStepNumber = 0;

    QWidget     *controlWidget = nullptr;
    QVBoxLayout *controlLayout = nullptr;
    QHBoxLayout *sphereLayout = nullptr;

    void createSideWidget();
    void createSphere();
    void createMenu();
    void createActions();
    void createStatusBar();
    void createTopBar();
    void createOpQueWidget();

    void startTimer();
    void stopTimer();

    void setEnabledWidgets(bool f);

    void nextAnimStepCircuit();

    static void  updateComplexLineEdit(QLineEdit *lineEdit);
    void         startMove(Vector *v, CurDecompFun getDec);
    void         startMove(Vector *v, Operator &op, CurDecompFun getDec);
    CurDecompFun getCurrentDecomposition();
    void         updateOp(OPERATOR_FORM exclude = OPERATOR_FORM::NOTHING);

    QWidget *makeDecompWid();
    QWidget *makeZyWid();
    QWidget *makeZxWid();
    QWidget *makeXyWid();
    QWidget *makeZyxWid();
    QWidget *makeOpWid();

    QTabWidget *topTabWid = nullptr;

    QPushButton *makeOpButton(const QString &str);
    QPushButton *appBut = nullptr;
    QPushButton *appQueBut = nullptr;
    QPushButton *clrQueBut = nullptr;

    QTabWidget *rxyzTab = nullptr;

    QLineEdit *rZyAlpEd = nullptr;
    QLineEdit *rZyBetEd = nullptr;
    QLineEdit *rZyGamEd = nullptr;
    QLineEdit *rZyDelEd = nullptr;

    QLineEdit *rZxAlpEd = nullptr;
    QLineEdit *rZxBetEd = nullptr;
    QLineEdit *rZxGamEd = nullptr;
    QLineEdit *rZxDelEd = nullptr;

    QLineEdit *rXyAlpEd = nullptr;
    QLineEdit *rXyBetEd = nullptr;
    QLineEdit *rXyGamEd = nullptr;
    QLineEdit *rXyDelEd = nullptr;

    QLineEdit *rZyxAlpEd = nullptr;
    QLineEdit *rZyxBetEd = nullptr;
    QLineEdit *rZyxGamEd = nullptr;
    QLineEdit *rZyxDelEd = nullptr;

    QTabWidget *stackW = nullptr;

    QLabel      *sphereLabel = nullptr;
    QPushButton *spherePlusBut = nullptr;
    QPushButton *sphereMinusBut = nullptr;

    QLabel      *speedLabel = nullptr;
    QPushButton *speedUpButton = nullptr;
    QPushButton *speedDownButton = nullptr;

    Operator curOperator;
    Operator singleOperator;

    QLineEdit *mat[2][2];

    QLineEdit    *axRnEd = nullptr;
    QLineEdit    *ngRnEd = nullptr;
    QRadioButton *rZyRb = nullptr;
    QRadioButton *rZxRb = nullptr;
    QRadioButton *rXyRb = nullptr;
    QRadioButton *rZyxRb = nullptr;
    QRadioButton *rtRb = nullptr;

    QLabel           *xyzStLab = nullptr;
    QLabel           *svdStLab = nullptr;
    QLabel           *mtnStLab = nullptr;
    QComboBox        *colorComboBox = nullptr;
    bool              isQueueAnimation = false;
    bool              isCircuitAnimation = false;
    QVector<OpItem *> opQueue;

    QListWidget *opQueWid = nullptr;

    QAction *aboutAct = nullptr;
    QAction *saveState = nullptr;
    QAction *recallState = nullptr;
    QAction *resetAct = nullptr;
    QAction *exitAct = nullptr;
    QAction *showTAct = nullptr;
    QAction *clearTAct = nullptr;
};

#endif // MAINWINDOW_HPP
