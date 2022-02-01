// Класс, реалзующий сферу Блоха.
// ------------------------------
// Кадочников Д. 4 курс (06.10.2014)

#ifndef BLOCH_H
#define BLOCH_H

#include "qubitvect.hpp"
#include <QGLWidget>
#include <QQueue>

enum rotType { rX, rY, rZ, rN }; // типы поворотов

struct QBVector : QObject, Qubit {
    Q_OBJECT
    // -----
    QVector<QBPoint> qbpVector; // вершины стрелки
    // ------------------------
    void fillQBPV(); // заполнение вершин стрелки
public:
    QBVector(float a_ = 1.0, comp_d b_ = comp_d(0.0, 0.0));
    void  setPsi(float a_, comp_d b_); // переопределение метода set
    void  set(float p_, float t_);     // переопределение метода set
    void  randomize();
    void  rotateX(float ng); // переопределение поворотов
    void  rotateY(float ng); // -------------------------
    void  rotateZ(float ng); // -------------------------
    void  rotation(float ng, rotType rt, float nX = 0, float nY = 0, float nZ = 0);
    void  draw(); // метод рисования
    void  report();
    void  reportOp(float nX, float nY, float nZ, float ng);
    void  applyQUOp(QUOperator &op);
    float _a() {
        return a;
    }
    comp_d _b() {
        return b;
    }
signals:
    void phiTheChanged(float, float);
    void alpBetChanged(float, comp_d);
    void newOp(QUOperator &op);
    void xyzChanged(float, float, float);
};

class Bloch : public QGLWidget {
    Q_OBJECT
    GLfloat xRot, yRot, zRot; // Углы поворота системы координат
    GLfloat nSca;             // Масштаб
    GLfloat sRadius;          // Радиус сферы
    QPoint  ptrMousePosition; // Позиция указателя мыши

    QBVector qubitVector; // кубит
    // --------------------------------
    // анимация вращения вектора
    // -------------------------
    // структура, описывающая поворот (ось вращения и угол)
    // ----------------------------------------------------
    struct rZYStruct {
        float   ang; // угол
        rotType typ; // тип
        float   naX, naY, naZ;
        QString msg;
        rZYStruct(float a = 0, rotType t = rX, QString ms = "", float ax = 0, float ay = 0,
                  float az = 0)
            : ang(a), typ(t), naX(ax), naY(ay), naZ(az) {
            if (ms.length())
                msg += ms + ": ";
            switch (t) {
            case rX:
                msg += QString("Rx(%1)").arg(okr(a));
                break;
            case rY:
                msg += QString("Ry(%1)").arg(okr(a));
                break;
            case rZ:
                msg += QString("Rz(%1)").arg(okr(a));
                break;
            case rN:
                msg += QString("Rn(%1); n(%2;%3;%4)")
                           .arg(okr(a))
                           .arg(okr(ax, 100))
                           .arg(okr(ay, 100))
                           .arg(okr(az, 100));
                break;
            }
        }
    };
    rZYStruct rZYCur; // текущие значения угла и типа поворота
    QQueue<rZYStruct> rZYQue; // очередь поворотов
    float             iter;
    int               timerID;       // таймер
    bool              isTimerActive; // флаг активности таймера
    int               trCol;
    QFont             font;
    // --------------------------------------------------

    QColor           oZCol, oYCol, oXCol; // Цвет осей
    QVector<GLfloat> tracePts;            // Список точек тректории
    bool             traceFlag;
    bool             showNewAx;
    float            naX, naY, naZ;

    void scale_plus();  // Методы изменения масштаба и вращения
    void scale_minus(); // осей координат
    void rotate_up();
    void rotate_down();
    void rotate_left();
    void rotate_right();
    void defaultScene();

    void drawVector(); // Отрисовка вектора
    void drawAxis();   // Отрисовка осей координат
    void drawCircle();
    void drawTrace();
    void drawProj();
    void showDecC();
    void drawNewAx();
    void drawRotInfo();
    void drawSphere(double r, int lats, int longs);

    void addTracePoint();
    void addTraceSeparator();

    void startMotion();
    void stopMotion();

protected:
    void initializeGL(); // Cтандартные методы OpenGL
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mousePressEvent(QMouseEvent *pe);
    void mouseMoveEvent(QMouseEvent *pe);
    void wheelEvent(QWheelEvent *pe);
    void keyPressEvent(QKeyEvent *pe);
    void timerEvent(QTimerEvent *pe);

signals:
    void motionBegin(QString);
    void motionEnd();

public:
    Bloch(QWidget *parent = 0);
    void setTP(float t = 0, float p = 0); // Установка положения вектора
    void setAB(float a = 0, comp_d b = comp_d(0, 0));
    void setRandomPsi();
    void rotateZY(float zb, float yg, float zd,
                  QString msg = ""); // ZY-вращение
    void rotateZX(float zb, float yx, float zd, QString msg = "");
    void rotateXY(float xb, float yg, float xd, QString msg = "");
    void setNewAxis(float ax, float ay, float az);
    void rotateN(float nt, QString msg = "");
    void hideNewAx() {
        showNewAx = false;
        updateGL();
    }
    void      reset();
    QBVector *getQBV() {
        return &qubitVector;
    }
    void setDrawTrace(bool st);
    void clearTrace();
    void setTraceColor(int n);
};

#endif // BLOCH_H
