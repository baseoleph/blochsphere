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
#include <QtMath>

#define RAD M_PI / 180.0
#define DEG 180.0 / M_PI
#define EEE 2.71828182845904523536
#define EPS 0.001
#define C_I complex(0, 1)

typedef QMap<Vector *, QVector<Sphere *>> MapVectors;

enum FIELD { NOTHIN = 0, THEPHI, ALPBET, BLOVEC };

struct Bloch {
    void hideNewAx();
    void reset();
};
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void addVector(Vector *v, MapVectors &mp);
    void removeVector(Vector *v, MapVectors &mp);
    void removeAllVectors(MapVectors &mp);

protected:
    void timerEvent(QTimerEvent *) override;

private:
    QVector<Sphere *> spheres;
    MapVectors        vectors;
    MapVectors        savedVectors;

    QWidget *controlWidget;
    void     createOldScene();
    void     setupOldControlBlock();

public slots:
    void slotThePhi();
    void slotAlpBet();
    void slotBloVec();
    void slotSetRandomPsi();

    void slotPhiTheChanged(float phi, float the);
    void slotAlpBetChanged(float a, complex b);
    void slotXYZChanged(float x, float y, float z);

    void slotButtonClicked();
    void slotSetMatrixOp();
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
    void slotDrawTrace();
    void slotClearTrace();
    void slotTraceColor(int);
    void slotTraceColor(QAction *);

    // TODO change about window
    static void slotAbout();

private:
    void createSideWidget();
    void createSphere();
    void createMenu();
    void createActions();
    void createStatusBar();
    void createTopBar();
    void createOpQueWidget();
    void fillFieldsOfVector(Spike sp, FIELD exclude = FIELD::NOTHIN);

    QWidget *makeThePhiWid();
    QWidget *makeAlpBetWid();
    QWidget *makeBloVecWid();
    QWidget *makeRXYZWid();
    QWidget *makeRZYWid();
    QWidget *makeRZXWid();
    QWidget *makeRXYWid();
    QWidget *makeOpWid();
    // ------------------
    QPushButton *makeOpButton(QString str);
    QPushButton *appBut;
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
    // rotZY
    QLineEdit *rZYAlpEd; // Поле ввода угла alpha
    QLineEdit *rZYBetEd; // Поле ввода угла beta
    QLineEdit *rZYGamEd; // Поле ввода угла gamma
    QLineEdit *rZYDelEd; // Поле ввода угла delta
    QLabel    *rZYLabel;
    // rotZX
    QLineEdit *rZXAlpEd; // Поле ввода угла alpha
    QLineEdit *rZXBetEd; // Поле ввода угла beta
    QLineEdit *rZXGamEd; // Поле ввода угла gamma
    QLineEdit *rZXDelEd; // Поле ввода угла delta
    QLabel    *rZXLabel;
    // rotXY
    QLineEdit *rXYAlpEd; // Поле ввода угла alpha
    QLineEdit *rXYBetEd; // Поле ввода угла beta
    QLineEdit *rXYGamEd; // Поле ввода угла gamma
    QLineEdit *rXYDelEd; // Поле ввода угла delta
    QLabel    *rXYLabel;
    // --------------
    QTabWidget *stackW;
    // ----------------
    Operator curOperator;
    QString  curOpName;
    // matrix
    QLineEdit *mat[2][2];
    // Rn
    QLineEdit    *axRnEd;
    QLineEdit    *ngRnEd;
    QRadioButton *rzyRB;
    QRadioButton *rzxRB;
    QRadioButton *rxyRB;
    QRadioButton *rtRB;
    // ----------------
    QLabel    *xyzStLab;
    QLabel    *abStLab;
    QLabel    *tpStLab;
    QLabel    *svdStLab;
    QLabel    *mtnStLab;
    double     savedAlp;
    complex    savedBet;
    QComboBox *qcomb;
    // --------------
    QListWidget *opQueWid;
    // ------------------
    QAction      *aboutAct;
    QAction      *saveState;
    QAction      *recallState;
    QAction      *resetAct;
    QAction      *exitAct;
    QAction      *applyAct;
    QAction      *addToQueAct;
    QAction      *drawTAct;
    QAction      *clearTAct;
    QActionGroup *rotMode;
    QActionGroup *trColor;
};

class AngInput : public QDialog {
    Q_OBJECT
    // -----
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

// TODO may be should create another file with useful functions and constants
// DOTO improve functions; increase accuracy
complex parseStrToComplex(const QString &str);
QString parseComplexToStr(complex c, int d = 10000);

#endif // MAINWINDOW_HPP
