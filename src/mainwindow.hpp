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

class QUOperator {};
struct Bloch {
    void hideNewAx();
    void reset();
};
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void function();

public slots:
    void addVector();
    void removeVector(Vector *v);
    void removeAllVectors();

protected:
    void timerEvent(QTimerEvent *);

private:
    QVector<Sphere *>                 spheres;
    QMap<Vector *, QVector<Sphere *>> vectors;

    QWidget *controlWidget;
    void     createOldScene();
    void     setupOldControlBlock();

public slots:
    // Ввод cостояния
    void slotThePhi();
    void slotAlpBet();
    void slotSetRandomPsi();
    // Изменение состояния
    void slotPhiTheChanged(float phi, float the);
    void slotAlpBetChanged(float a, complex b);
    void slotXYZChanged(float x, float y, float z);
    // Ввод оператора
    void slotButtonClicked();
    void slotSetMatrixOp();
    void slotSetRXYZOp();
    void slotSetNewAxOp();
    //    void slotHideNewAx() {
    //        sceneOld->hideNewAx();
    //    }
    void slotNewOp(QUOperator &op);
    void slotQueItemClicked(QListWidgetItem *it);
    void slotOpItemDelete();
    // Применение оператора
    void slotApplyOp();
    void slotAddToQue();
    void slotApplyQue();
    // -------------------
    //    void slotReset() {
    //        sceneOld->reset();
    //    }
    void slotSaveState();
    void slotRecallState();
    void slotMotionBegin(QString);
    void slotMotionEnd();
    void slotDrawTrace();
    void slotClearTrace();
    void slotTraceColor(int);
    void slotTraceColor(QAction *);

    // TODO change about window
    void slotAbout();

private:
    void createSideWidget();
    void createSphere();
    void createMenu();
    void createActions();
    void createStatusBar();
    void createTopBar();
    void createOpQueWidget();

    QWidget *makeThePhiWid();
    QWidget *makeAlpBetWid();
    QWidget *makeRXYZWid();
    QWidget *makeRZYWid();
    QWidget *makeRZXWid();
    QWidget *makeRXYWid();
    QWidget *makeOpWid();
    // ------------------
    QPushButton *makeOpButton(QString str); // Создание кнопки оператора
    double       phiFun(double the, double re, double im); // Вычисление угла фи
    void         updateOp();
    //--------------
    Bloch *scene; // Виджет со сферой Блоха
    // phi the
    QLineEdit *theEd; // Поле ввода угла тэта
    QLineEdit *phiEd; // Поле ввода угла фи
    // alpha beta
    QLineEdit *alpEd;   // Поле ввода alpha
    QLineEdit *reBetEd; // Поле ввода Re(beta)
    // ----------------------------------------
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
    QUOperator curOperator;
    QString    curOpName;
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
    QUOperator oper;

public:
    OpItem(QString str, QUOperator op);
    QUOperator getOp();
};

complex parceC(QString str);
QString deparceC(complex c, int d = 10000);

#endif // MAINWINDOW_HPP
