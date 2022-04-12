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

#include "qubit.hpp"
#include <QRandomGenerator64>
#include <iostream>
#include <ostream>

const quint64 SEED = QRandomGenerator::global()->generate64();

struct matrix2x2 {
    complex a;
    complex b;
    complex c;
    complex d;
};

class UnitaryMatrix2x2 {
public:
    explicit UnitaryMatrix2x2() : _matrix({1, 0, 0, 1}) {}

    bool             updateMatrix(matrix2x2 matrix);
    static bool      isUnitaryMatrix(UnitaryMatrix2x2 op);
    static bool      isUnitaryMatrix(matrix2x2 matrix);
    static bool      compareOperators(UnitaryMatrix2x2 op1, UnitaryMatrix2x2 op2);
    UnitaryMatrix2x2 getConjugateTranspose();
    static bool      fuzzyCompare(double a, double b);
    void             print(std::ostream &out) const;

    [[nodiscard]] complex a() const { return _matrix.a; }
    [[nodiscard]] complex b() const { return _matrix.b; }
    [[nodiscard]] complex c() const { return _matrix.c; }
    [[nodiscard]] complex d() const { return _matrix.d; }

    static UnitaryMatrix2x2 getX();
    static UnitaryMatrix2x2 getY();
    static UnitaryMatrix2x2 getZ();
    static UnitaryMatrix2x2 getH();
    static UnitaryMatrix2x2 getS();
    static UnitaryMatrix2x2 getT();

private:
    matrix2x2 _matrix;

    // Matrices must be unitary
    explicit UnitaryMatrix2x2(matrix2x2 matrix) : _matrix(matrix) {}
    UnitaryMatrix2x2 operator*(const UnitaryMatrix2x2 &op);
};
#endif // UNITARYMATRIX2X2_HPP
