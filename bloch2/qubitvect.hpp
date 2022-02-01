// Класс, моделирующий кубит, как точку на сфере единичного радиуса
// ----------------------------------------------------------------
// Кадочников Д. 4 курс (13.10.2014)

#include<qmath.h>
#include<complex>
#include<qvector.h>
#include<vector>

#ifndef QUBITVECT
#define QUBITVECT
#define RAD       M_PI/180.0
#define DEG       180.0/M_PI
#define EEE       2.71828182845904523536
#define EPS       0.001  

typedef std::complex<double> comp_d;
const comp_d C_E=comp_d(EEE,0.0);
const comp_d C_I=comp_d(0.0,1.0);

struct QUOperator
// Однокубитовый унитарный опреатор
// --------------------------------
{
    QUOperator(comp_d a_=1.0,
               comp_d b_=0.0,
               comp_d c_=0.0,
               comp_d d_=1.0);
    void set(comp_d a_=1.0,
             comp_d b_=0.0,
             comp_d c_=0.0,
             comp_d d_=1.0);
    double _alp() const {return alpha;}
    double _bet() const {return beta;}
    double _gam() const {return gamma;}
    double _del() const {return delta;}
    comp_d  _a() const {return mA;}
    comp_d  _b() const {return mB;}
    comp_d  _c() const {return mC;}
    comp_d  _d() const {return mD;}
    bool isUnitary();
    QUOperator operator+(QUOperator &op) const;
    QUOperator operator-(QUOperator &op) const;
    QUOperator operator*(double n) const;
    friend QUOperator operator*(const double n, const QUOperator &op);
    QUOperator QUOperator::operator*(comp_d n) const;
    friend QUOperator operator*(const comp_d n,const QUOperator &op);
    QVector<double> findNVec();
    void toZYdec();
    void toZXdec();
    void toXYdec();
private:
    comp_d mA,mB,mC,mD; // матричное представление
    double alpha,beta,gamma,delta; // RY-разложение
};

struct QBPoint
// точка в полярной системе координат
// ----------------------------------
{
    QBPoint(double p_=0.0, double t_=0.0);
    QBPoint(double x_, double y_, double z_);
    void set(double phi_, double the_);
    void rotateX(double ng);  // поворот вокруг ОХ
    void rotateY(double ng);  // поворот вокруг ОУ
    void rotateZ(double ng);  // поворот вокруг OZ
    double _x(){return x;}
    double _y(){return y;}
    double _z(){return z;}
    double _phi(){return phi;}
    double _the(){return the;}

protected:
    double x,y,z;     // прямоугольные кooрдинаты
    double the,phi;   // полярные углы
    // -----------
    void evalPT();   // вычислить полярные углы
    void evalXYZ();  // вычислить координаты
};


struct Qubit : QBPoint
// представление кубита как точки на сфере 
// единичного радиуса
// ------------------
{
    Qubit(double a_= 1.0, comp_d b_= 0.0);  // конструктор
    void setPsi(double a_, comp_d b_);
    void set(double phi_, double the_);
    void randomize();        // задать случайно
    void rotateX(double ng);  // поворот вокруг ОХ
    void rotateY(double ng);  // поворот вокруг ОУ
    void rotateZ(double ng);  // поворот вокруг OZ
    void applyQUOp(QUOperator &op);
protected:
    void evalVertex();     // вычислить координаты
    void evalAB();         // вычислить полярные углы
    // --------------------------------------------
    double a;                // число а
    std::complex<double> b;  // число b
};


// -------------------------------------
//          Основные операторы
// -------------------------------------
const QUOperator Hop(1.0/sqrt(2.0), 1.0/sqrt(2.0),
                     1.0/sqrt(2.0),-1.0/sqrt(2.0));
const QUOperator Xop(0.0,1.0,1.0,0.0);
const QUOperator Yop(0.0,-C_I,C_I,0.0);
const QUOperator Zop(1.0,0.0,0.0,-1.0);
const QUOperator Sop(1.0,0.0,0.0,C_I);
const QUOperator Top(1.0,0.0,0.0,pow(comp_d(EEE,0.0),comp_d(0.0,1.0)*
                                     comp_d(M_PI,0.0)/comp_d(4.0,0.0))); 
const QUOperator Iop(1.0,0.0,0.0,1.0); 

// -------------------------------------
//       Вспомогательгые функции
// -------------------------------------
QUOperator RXop(double the);
QUOperator RYop(double the);
QUOperator RZop(double the);
QUOperator PHIop(double gam);

double okr(double a, double s=1);
bool eq(comp_d a, comp_d b);

#endif