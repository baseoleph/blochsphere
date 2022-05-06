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

#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "UnitaryMatrix2x2.h"
#include "Vector.h"
#include "src/utility.h"

#if QT_VERSION >= 0x050000
#include <QRandomGenerator64>
#include <QtMath>
#else
#include <QtCore/qmath.h>
#include <cstdlib>
#endif

struct decomposition {
    double alpha = 0; // rad
    double beta = 0;  // rad
    double delta = 0; // rad
    double gamma = 0; // rad

    void print(std::ostream &out) const {
        out << "----------------------------------------------\n";
        out << "alpha: " << alpha * 180 / M_PI << "\n";
        out << "beta: " << beta * 180 / M_PI << "\n";
        out << "delta: " << delta * 180 / M_PI << "\n";
        out << "gamma: " << gamma * 180 / M_PI << "\n";
        out << "----------------------------------------------\n";
    }
};

struct vectorangle {
    double x = 0;
    double y = 0;
    double z = 0;
    double angle = 0; // rad
};

class Operator {
public:
    Operator();
    static QVector<Spike> rotate(Spike s, QVector3D v, double gamma);
    static QVector<Spike> rXRotate(Spike s, double gamma);
    static QVector<Spike> rYRotate(Spike s, double gamma);
    static QVector<Spike> rZRotate(Spike s, double gamma);

    static QVector<Spike> applyZxDecomposition(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyZxDecomposition(Spike s);
    static QVector<Spike> applyZyDecomposition(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyZyDecomposition(Spike s);
    static QVector<Spike> applyXyDecomposition(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyXyDecomposition(Spike s);
    static QVector<Spike> applyZyxDecomposition(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyZyxDecomposition(Spike s);
    static QVector<Spike> applyOperator(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyOperator(Spike s);
    static QVector<Spike> applyVectorRotation(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyVectorRotation(Spike s);

    static decomposition zxDecomposition(UnitaryMatrix2x2 op);
    decomposition        zxDecomposition();
    static decomposition zyDecomposition(UnitaryMatrix2x2 op);
    decomposition        zyDecomposition();
    static decomposition xyDecomposition(UnitaryMatrix2x2 op);
    decomposition        xyDecomposition();
    static decomposition zyxDecomposition(UnitaryMatrix2x2 op);
    decomposition        zyxDecomposition();
    static vectorangle   vectorAngleDec(UnitaryMatrix2x2 op);
    vectorangle          vectorAngleDec();

    static matrix2x2 getMatrixByZxDec(decomposition dec);
    static matrix2x2 getMatrixByZyDec(decomposition dec);
    static matrix2x2 getMatrixByXyDec(decomposition dec);
    static matrix2x2 getMatrixByZyxDec(decomposition dec);
    static matrix2x2 getMatrixByVecAng(vectorangle va);

    bool setOperatorByZxDecomposition(decomposition dec);
    bool setOperatorByZyDecomposition(decomposition dec);
    bool setOperatorByXyDecomposition(decomposition dec);
    bool setOperatorByZyxDecomposition(decomposition dec);
    bool setOperatorByVectorAngle(vectorangle va);
    void setOperator(UnitaryMatrix2x2 op, QString opName = "U");

    static UnitaryMatrix2x2 genRandUnitaryMatrix(qint64 seed = 0);

    UnitaryMatrix2x2 getOperator() { return _op; }

    static QString getCurOperatorMatrixStr(UnitaryMatrix2x2 op);
    QString        getCurOperatorMatrixStr();
    static QString getOperatorName(UnitaryMatrix2x2 op);
    QString        getOperatorName();

    void toX();
    void toY();
    void toZ();
    void toH();
    void toS();
    void toT();
    void toPhi(double gamma);
    void toXrotate(double the);
    void toYrotate(double the);
    void toZrotate(double the);
    void toId();
    void toRandUnitaryMatrix();

    complex a() { return _op.a(); }
    complex b() { return _op.b(); }
    complex c() { return _op.c(); }
    complex d() { return _op.d(); }

private:
    UnitaryMatrix2x2 _op;
    QString          _opName;
};

#endif // OPERATOR_HPP
