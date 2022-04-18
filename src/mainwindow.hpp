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

#include "blochUtility.h"
#include "operator.hpp"
#include "qubit.hpp"
#include "sphere.hpp"
#include <QActionGroup>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QRadioButton>
#include <QVector>

typedef QMap<Vector *, QVector<Sphere *>> MapVectors;

enum FIELD { NOTHIN = 0, THEPHI, ALPBET, BLOVEC };

typedef QVector<Spike> (Operator::*CurDecompFun)(Spike);

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void addVector(Vector *v, MapVectors &mp);
    void addVector(Vector *v, MapVectors &mp, Sphere *sph);
    void removeVector(Vector *v, MapVectors &mp);
    void removeAllVectors(MapVectors &mp);

protected:
    void timerEvent(QTimerEvent *) override;

private:
    QVector<Sphere *> spheres;
    MapVectors        vectors;
    MapVectors        savedVectors;

    QWidget *controlWidget;

public slots:
    void slotThePhi();
    void slotAlpBet();
    void slotBloVec();
    void slotSetRandomPsi();

    void slotSetOperatorClicked();
    void slotSetMatrixOp();
    void slotSetRandomOp();
    void slotSetRXYZOp();
    void slotSetNewAxOp();
    //    void slotHideNewAx() {
    //        sceneOld->hideNewAx();
    //    }
    void slotNewOp(Operator &op);
    void slotQueItemClicked(QListWidgetItem *it);
    void slotOpItemDelete();

    void slotApplyOp();
    void slotAddToQue();
    void slotApplyQue();

    void slotReset();
    void slotSaveState();
    void slotRecallState();
    void slotMotionBegin(QString);
    void slotMotionEnd();
    void slotShowTrace();
    void slotClearTrace();
    void slotTraceColor(int index);

    void slotComplexLineEditChanged(const QString &);
    // TODO change about window
    static void slotAbout();

private:
    void         createSideWidget();
    void         createSphere();
    void         createMenu();
    void         createActions();
    void         createStatusBar();
    void         createTopBar();
    void         createOpQueWidget();
    void         fillFieldsOfVector(Spike sp, FIELD exclude = FIELD::NOTHIN);
    void         updateComplexLineEdit(QLineEdit *lineEdit);
    void         startMove(Vector *v, CurDecompFun getDec);
    CurDecompFun getCurrentDecomposition();
    void         updateCurOperatorTable();

    QWidget *makeThePhiWid();
    QWidget *makeAlpBetWid();
    QWidget *makeBloVecWid();
    QWidget *makeRXYZWid();
    QWidget *makeRZYWid();
    QWidget *makeRZXWid();
    QWidget *makeRXYWid();
    QWidget *makeOpWid();

    QPushButton *makeOpButton(QString str);
    QPushButton *appBut;
    QPushButton *appQueBut;
    QLabel      *currentOperatorLabel;
    double       phiFun(double the, double re, double im);
    void         updateOp();

    QLineEdit *theEd;
    QLineEdit *phiEd;

    QLineEdit *alpEd;
    QLineEdit *reBetEd;

    QLineEdit *xEd;
    QLineEdit *yEd;
    QLineEdit *zEd;

    QTabWidget *rxyzTab;

    QLineEdit *rZYAlpEd;
    QLineEdit *rZYBetEd;
    QLineEdit *rZYGamEd;
    QLineEdit *rZYDelEd;
    QLabel    *rZYLabel;

    QLineEdit *rZXAlpEd;
    QLineEdit *rZXBetEd;
    QLineEdit *rZXGamEd;
    QLineEdit *rZXDelEd;
    QLabel    *rZXLabel;

    QLineEdit *rXYAlpEd;
    QLineEdit *rXYBetEd;
    QLineEdit *rXYGamEd;
    QLineEdit *rXYDelEd;
    QLabel    *rXYLabel;

    QTabWidget *stackW;

    Operator curOperator;
    Operator singleOperator;
    QString  curOpName;

    QLineEdit *mat[2][2];

    QLineEdit    *axRnEd;
    QLineEdit    *ngRnEd;
    QRadioButton *rzyRB;
    QRadioButton *rzxRB;
    QRadioButton *rxyRB;
    QRadioButton *rtRB;

    QLabel           *xyzStLab;
    QLabel           *abStLab;
    QLabel           *tpStLab;
    QLabel           *svdStLab;
    QLabel           *mtnStLab;
    double            savedAlp;
    complex           savedBet;
    QComboBox        *qcomb;
    bool              isQueueAnimation = false;
    QVector<Operator> opQueue = {};

    QListWidget *opQueWid;

    QAction *aboutAct;
    QAction *saveState;
    QAction *recallState;
    QAction *resetAct;
    QAction *exitAct;
    QAction *showTAct;
    QAction *clearTAct;
};

class AngInput : public QDialog {
    Q_OBJECT
    QLineEdit *angEd;

public:
    AngInput(QWidget *pwgt = 0);
    QString ang() const;
};

class OpItem : public QListWidgetItem {
    Operator oper;

public:
    OpItem(QString str, Operator op);
    Operator getOp();
};

#endif // MAINWINDOW_HPP
