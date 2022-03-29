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

#ifndef UNITARYMATRIX2X2_HPP
#define UNITARYMATRIX2X2_HPP

#include "qubit.h"
#include <QRandomGenerator64>
#include <iostream>
#include <ostream>

const quint64 SEED = QRandomGenerator::global()->generate64();

class UnitaryMatrix2x2 {
public:
    explicit UnitaryMatrix2x2() : _a(1), _b(0), _c(0), _d(1) {
    }

    bool             updateMatrix(complex a, complex b, complex c, complex d);
    static bool      isUnitaryMatrix(UnitaryMatrix2x2 op);
    static bool      isUnitaryMatrix(complex a, complex b, complex c, complex d);
    static bool      compareOperators(UnitaryMatrix2x2 op1, UnitaryMatrix2x2 op2);
    UnitaryMatrix2x2 getConjugateTranspose();
    static bool      fuzzyCompare(double a, double b);
    void             print(std::ostream &out);

    complex a() const {
        return _a;
    }
    complex b() const {
        return _b;
    }
    complex c() const {
        return _c;
    }
    complex d() const {
        return _d;
    }

private:
    complex _a;
    complex _b;
    complex _c;
    complex _d;

    // Matrices must be unitary
    UnitaryMatrix2x2(complex a, complex b, complex c, complex d) : _a(a), _b(b), _c(c), _d(d) {
    }
    UnitaryMatrix2x2 operator*(const UnitaryMatrix2x2 &op);
};
#endif // UNITARYMATRIX2X2_HPP
