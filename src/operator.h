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

#ifndef OPERATOR_H
#define OPERATOR_H

#include "vector.h"
#include <QtMath>

// TODO change logic duration of animation
#define DURATION 100.

struct unitaryMatrix {
    complex a;
    complex b;
    complex c;
    complex d;
};

struct decomposition {
    double alpha = 0;
    double beta = 0;
    double delta = 0;
    double gamma = 0;
};

// TODO check for unitary
class Operator {
public:
    Operator();
    static QVector<Spike> rotate(Spike s, QVector3D v, double gamma);
    static QVector<Spike> rXRotate(Spike s, double gamma);
    static QVector<Spike> rYRotate(Spike s, double gamma);
    static QVector<Spike> rZRotate(Spike s, double gamma);
    static QVector<Spike> applyZXDecomposition(Spike s, unitaryMatrix op);
    static QVector<Spike> applyOperator(Spike s, unitaryMatrix op);

    // TODO it's private method
    static decomposition zxDecomposition(unitaryMatrix op);
};

#endif // OPERATOR_H
