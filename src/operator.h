#ifndef OPERATOR_H
#define OPERATOR_H

#include "qubit.h"
#include <QtMath>

#define DURATION 100.

typedef QVector<QVector<complex>> operator2d;
class Operator {
public:
    Operator();
    static Qubit          actByOperator(operator2d op, Qubit q);
    static QVector<Qubit> rXRotate(Qubit q, double gamma);
    static QVector<Qubit> rYRotate(Qubit q, double gamma);
    static QVector<Qubit> rZRotate(Qubit q, double gamma);
    static QVector<Qubit> zxRotate(Qubit q, double a, double b, double g, double d);
    static operator2d     getRX(double gamma);
    static operator2d     getRY(double gamma);
    static operator2d     getRZ(double gamma);
    static void           printOperator(operator2d op);
};

#endif // OPERATOR_H
