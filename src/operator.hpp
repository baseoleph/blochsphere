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

#include "blochUtility.h"
#include "unitaryMatrix2x2.hpp"
#include "vector.hpp"
#include <QtMath>

struct decomposition {
    double alpha = 0; // deg
    double beta = 0;  // deg
    double delta = 0; // deg
    double gamma = 0; // deg

    void print(std::ostream &out) const {
        out << "----------------------------------------------\n";
        out << "alpha: " << alpha << "\n";
        out << "beta: " << beta << "\n";
        out << "delta: " << delta << "\n";
        out << "gamma: " << gamma << "\n";
        out << "----------------------------------------------\n";
    }
};

class Operator {
public:
    Operator();
    static QVector<Spike> rotate(Spike s, QVector3D v, double gamma);
    static QVector<Spike> rXRotate(Spike s, double gamma);
    static QVector<Spike> rYRotate(Spike s, double gamma);
    static QVector<Spike> rZRotate(Spike s, double gamma);

    static QVector<Spike> applyZXDecomposition(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyZXDecomposition(Spike s);
    static QVector<Spike> applyOperator(Spike s, UnitaryMatrix2x2 op);
    QVector<Spike>        applyOperator(Spike s);

    static decomposition zxDecomposition(UnitaryMatrix2x2 op);
    decomposition        zxDecomposition();
    static decomposition zyDecomposition(UnitaryMatrix2x2 op);
    decomposition        zyDecomposition();

    static matrix2x2 getMatrixByZxDec(decomposition dec);
    static matrix2x2 getMatrixByZyDec(decomposition dec);

    bool setOperatorByZXDecomposition(decomposition dec);
    void setOperator(UnitaryMatrix2x2 op);

    static UnitaryMatrix2x2 genRandUnitaryMatrix(qint64 seed);

    UnitaryMatrix2x2 getOperator() { return _op; }
    void             toX();
    void             toY();
    void             toZ();
    void             toH();
    void             toS();
    void             toT();
    void             toId();
    complex          a() { return _op.a(); }
    complex          b() { return _op.b(); }
    complex          c() { return _op.c(); }
    complex          d() { return _op.d(); }

private:
    UnitaryMatrix2x2 _op;
};

#endif // OPERATOR_HPP
