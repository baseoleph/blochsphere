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
#include "opitem.hpp"
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

    void slotThePhi();
    void slotAlpBet();
    void slotBloVec();
    void slotSetRandomPsi();

    void slotSetOperatorClicked();
    void slotSetMatrixOp();
    void slotSetRandomOp();
    void slotSetRXYZOp();
    void slotSetNewAxOp();
    void slotQueItemClicked(QListWidgetItem *it);
    void slotOpItemDelete();

    void slotApplyOp();
    void slotAddToQue();
    void slotApplyQue();

    void slotReset();
    void slotSaveState();
    void slotRecallState();
    void slotShowTrace();
    void slotClearTrace();
    void slotTraceColor(int index);

    void slotComplexLineEditChanged(const QString &);
    // TODO change about window
    static void slotAbout();

protected:
    void timerEvent(QTimerEvent *) override;

private:
    QVector<Sphere *> spheres;
    MapVectors        vectors;
    MapVectors        savedVectors;

    QWidget *controlWidget = nullptr;

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
    void         updateOp();

    QWidget *makeThePhiWid();
    QWidget *makeAlpBetWid();
    QWidget *makeBloVecWid();
    QWidget *makeRXYZWid();
    QWidget *makeRZYWid();
    QWidget *makeRZXWid();
    QWidget *makeRXYWid();
    QWidget *makeOpWid();

    QPushButton *makeOpButton(QString str);
    QPushButton *appBut = nullptr;
    QPushButton *appQueBut = nullptr;
    QLabel      *currentOperatorLabel = nullptr;

    QLineEdit *theEd = nullptr;
    QLineEdit *phiEd = nullptr;

    QLineEdit *alpEd = nullptr;
    QLineEdit *betEd = nullptr;

    QLineEdit *xEd = nullptr;
    QLineEdit *yEd = nullptr;
    QLineEdit *zEd = nullptr;

    QTabWidget *rxyzTab = nullptr;

    QLineEdit *rZYAlpEd = nullptr;
    QLineEdit *rZYBetEd = nullptr;
    QLineEdit *rZYGamEd = nullptr;
    QLineEdit *rZYDelEd = nullptr;

    QLineEdit *rZXAlpEd = nullptr;
    QLineEdit *rZXBetEd = nullptr;
    QLineEdit *rZXGamEd = nullptr;
    QLineEdit *rZXDelEd = nullptr;

    QLineEdit *rXYAlpEd = nullptr;
    QLineEdit *rXYBetEd = nullptr;
    QLineEdit *rXYGamEd = nullptr;
    QLineEdit *rXYDelEd = nullptr;

    QTabWidget *stackW = nullptr;

    Operator curOperator;
    Operator singleOperator;
    QString  curOpName;

    QLineEdit *mat[2][2];

    QLineEdit    *axRnEd = nullptr;
    QLineEdit    *ngRnEd = nullptr;
    QRadioButton *rzyRB = nullptr;
    QRadioButton *rzxRB = nullptr;
    QRadioButton *rxyRB = nullptr;
    QRadioButton *rtRB = nullptr;

    QLabel           *xyzStLab = nullptr;
    QLabel           *svdStLab = nullptr;
    QLabel           *mtnStLab = nullptr;
    QComboBox        *colorComboBox = nullptr;
    bool              isQueueAnimation = false;
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
