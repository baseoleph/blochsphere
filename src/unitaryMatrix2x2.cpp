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

#include "unitaryMatrix2x2.hpp"

bool UnitaryMatrix2x2::updateMatrix(complex a, complex b, complex c, complex d) {
    UnitaryMatrix2x2 tmpOp(a, b, c, d);
    if (isUnitaryMatrix(tmpOp)) {
        _a = a;
        _b = b;
        _c = c;
        _d = d;

        return true;
    }

    return false;
}

bool UnitaryMatrix2x2::isUnitaryMatrix(UnitaryMatrix2x2 op1) {
    return compareOperators(UnitaryMatrix2x2(), op1 * op1.getConjugateTranspose());
}

bool UnitaryMatrix2x2::isUnitaryMatrix(complex a, complex b, complex c, complex d) {
    return isUnitaryMatrix(UnitaryMatrix2x2(a, b, c, d));
}

bool UnitaryMatrix2x2::fuzzyCompare(double a, double b) { return qAbs(a - b) <= EPSILON; }

void UnitaryMatrix2x2::print(std::ostream &out) {
    out << "----------------------------------------------\n";
    out << "(" << a().real() << ", " << a().imag() << ") \t";
    out << "(" << b().real() << ", " << b().imag() << ") \n";
    out << "(" << c().real() << ", " << c().imag() << ") \t";
    out << "(" << d().real() << ", " << d().imag() << ") \n";
    out << "----------------------------------------------\n";
}

bool UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2 op1, UnitaryMatrix2x2 op2) {
    bool cmp = true;
    cmp &= fuzzyCompare(op1.a().real(), op2.a().real());
    cmp &= fuzzyCompare(op1.a().imag(), op2.a().imag());

    cmp &= fuzzyCompare(op1.b().real(), op2.b().real());
    cmp &= fuzzyCompare(op1.b().imag(), op2.b().imag());

    cmp &= fuzzyCompare(op1.c().real(), op2.c().real());
    cmp &= fuzzyCompare(op1.c().imag(), op2.c().imag());

    cmp &= fuzzyCompare(op1.d().real(), op2.d().real());
    cmp &= fuzzyCompare(op1.d().imag(), op2.d().imag());

    if (!cmp) {
        std::cout << "----------------------------------------------\n";
        std::cout << "Expected\n";
        std::cout << "(" << op1.a().real() << ", " << op1.a().imag() << ") \t";
        std::cout << "(" << op1.b().real() << ", " << op1.b().imag() << ") \n";
        std::cout << "(" << op1.c().real() << ", " << op1.c().imag() << ") \t";
        std::cout << "(" << op1.d().real() << ", " << op1.d().imag() << ") \n";
        std::cout << "\nActual\n";
        std::cout << "(" << op2.a().real() << ", " << op2.a().imag() << ") \t";
        std::cout << "(" << op2.b().real() << ", " << op2.b().imag() << ") \n";
        std::cout << "(" << op2.c().real() << ", " << op2.c().imag() << ") \t";
        std::cout << "(" << op2.d().real() << ", " << op2.d().imag() << ") \n";
        std::cout << "----------------------------------------------\n";
    }

    return cmp;
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getConjugateTranspose() {
    return UnitaryMatrix2x2(conj(_a), conj(_c), conj(_b), conj(_d));
}

UnitaryMatrix2x2 UnitaryMatrix2x2::operator*(const UnitaryMatrix2x2 &op) {
    complex a = _a * op.a() + _b * op.c();
    complex b = _a * op.b() + _b * op.d();
    complex c = _c * op.a() + _d * op.c();
    complex d = _c * op.b() + _d * op.d();
    return UnitaryMatrix2x2(a, b, c, d);
}

// TODO why I use curly braces?
UnitaryMatrix2x2 UnitaryMatrix2x2::getX() { return UnitaryMatrix2x2{0, 1, 1, 0}; }
UnitaryMatrix2x2 UnitaryMatrix2x2::getY() {
    return UnitaryMatrix2x2{0, -complex(0, 1), complex(0, 1), 0};
}
UnitaryMatrix2x2 UnitaryMatrix2x2::getZ() { return UnitaryMatrix2x2{1, 0, 0, -1}; }
UnitaryMatrix2x2 UnitaryMatrix2x2::getH() {
    double h = 1 / sqrt(2);
    return UnitaryMatrix2x2{h, h, h, -h};
}
UnitaryMatrix2x2 UnitaryMatrix2x2::getS() { return UnitaryMatrix2x2{1, 0, 0, complex(0, 1)}; }
UnitaryMatrix2x2 UnitaryMatrix2x2::getT() {
    return UnitaryMatrix2x2{1, 0, 0, exp(complex(0, 1) * (M_PI / 4))};
}
