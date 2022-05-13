// Класс главного окна.
// --------------------
// Кадочников Д. 4 курс (06.10.2014)

#ifndef MAINWID_H
#define MAINWID_H

#include <QMainWindow>
#include <QDialog>
#include <QListWidget>
#include "bloch.h"

class QLineEdit;
class QLabel;
class QRadioButton;
class QPushButton;
class QActionGroup;
class QComboBox;
class OpItem;

class MainWid : public QMainWindow{
    Q_OBJECT
   // ------

public:
    MainWid(QWidget *parent = 0, Qt::WFlags flags = 0);

public slots:
    // Ввод cостояния
    void slotThePhi();
    void slotAlpBet();
    void slotSetRandomPsi();
    // Изменение состояния
    void slotPhiTheChanged(float phi,float the); 
    void slotAlpBetChanged(float a,comp_d b);
    void slotXYZChanged(float x, float y, float z);
    // Ввод оператора
    void slotButtonClicked();
    void slotSetMatrixOp();
    void slotSetRXYZOp();
    void slotSetNewAxOp();
    void slotHideNewAx(){scene->hideNewAx();}
    void slotNewOp(QUOperator &op);
    void slotQueItemClicked(QListWidgetItem *it);
    void slotOpItemDelete();
    // Применение оператора
    void slotApplyOp();
    void slotAddToQue();
    void slotApplyQue();
    // -------------------
    void slotReset(){scene->reset();}
    void slotSaveState();
    void slotRecallState();
    void slotMotionBegin(QString);
    void slotMotionEnd();
    void slotDrawTrace();
    void slotClearTrace();
    void slotTraceColor(int);
    void slotTraceColor(QAction*);
    void slotAbout();

private:
    void createSideWidget();
    void createSphere();
    void createMenu();
    void createActions();
    void createStatusBar();
    void createTopBar();
    void createOpQueWidget();
    QWidget* makeThePhiWid();
    QWidget* makeAlpBetWid();
    QWidget *makeRXYZWid();
    QWidget* makeRZYWid();
    QWidget* makeRZXWid();
    QWidget* makeRXYWid();
    QWidget* makeOpWid();
    // ------------------
    QPushButton *makeOpButton(QString str); // Создание кнопки оператора
    double phiFun(double the, double re, double im); // Вычисление угла фи
    void updateOp();
    //--------------
    Bloch *scene;        // Виджет со сферой Блоха
    // phi the
    QLineEdit *theEd;    // Поле ввода угла тэта
    QLineEdit *phiEd;    // Поле ввода угла фи
    // alpha beta
    QLineEdit *alpEd;    // Поле ввода alpha
    QLineEdit *reBetEd;  // Поле ввода Re(beta)
    // ----------------------------------------
    QTabWidget *rxyzTab;
    // rotZY
    QLineEdit *rZYAlpEd;  // Поле ввода угла alpha
    QLineEdit *rZYBetEd;  // Поле ввода угла beta
    QLineEdit *rZYGamEd;  // Поле ввода угла gamma
    QLineEdit *rZYDelEd;  // Поле ввода угла delta
    QLabel *rZYLabel;
    // rotZX
    QLineEdit *rZXAlpEd;  // Поле ввода угла alpha
    QLineEdit *rZXBetEd;  // Поле ввода угла beta
    QLineEdit *rZXGamEd;  // Поле ввода угла gamma
    QLineEdit *rZXDelEd;  // Поле ввода угла delta
    QLabel *rZXLabel;
    // rotXY
    QLineEdit *rXYAlpEd;  // Поле ввода угла alpha
    QLineEdit *rXYBetEd;  // Поле ввода угла beta
    QLineEdit *rXYGamEd;  // Поле ввода угла gamma
    QLineEdit *rXYDelEd;  // Поле ввода угла delta
    QLabel *rXYLabel;
    // --------------
    QTabWidget *stackW;
    // ----------------
    QUOperator curOperator;
    QString curOpName;
    // matrix
    QLineEdit *mat[2][2];
    // Rn
    QLineEdit *axRnEd;
    QLineEdit *ngRnEd;
    QRadioButton *rzyRB;
    QRadioButton *rzxRB;
    QRadioButton *rxyRB;
    QRadioButton *rtRB;
    // ----------------
    QLabel *xyzStLab;
    QLabel *abStLab;
    QLabel *tpStLab;
    QLabel *svdStLab;
    QLabel *mtnStLab;
    double savedAlp;
    comp_d savedBet;
    QComboBox *qcomb;
    // --------------
    QListWidget *opQueWid;
    // ------------------
    QAction *aboutAct;
    QAction *saveState;
    QAction *recallState;
    QAction *resetAct;
    QAction *exitAct;
    QAction *applyAct;
    QAction *addToQueAct;
    QAction *drawTAct;
    QAction *clearTAct;
    QActionGroup *rotMode;
    QActionGroup *trColor;

};

class AngInput : public QDialog
{
    Q_OBJECT
    // -----
    QLineEdit *angEd;

public:
    AngInput(QWidget *pwgt = 0);
    QString ang() const;
};

class OpItem : public QListWidgetItem
{
    QUOperator oper;
public:
    OpItem(QString str, QUOperator op);
    QUOperator getOp();

};


// -------------------------------------
//       Вспомогательгые функции
// -------------------------------------

comp_d parceC(QString str);
QString deparceC(comp_d c,int d=10000);


#endif // MAINWID_H

