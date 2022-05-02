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

bool UnitaryMatrix2x2::updateMatrix(matrix2x2 matrix) {
    if (isUnitaryMatrix(matrix)) {
        _matrix = matrix;
        return true;
    }
    return false;
}

bool UnitaryMatrix2x2::isUnitaryMatrix(UnitaryMatrix2x2 op1) {
    return compareOperators(UnitaryMatrix2x2(), op1 * op1.getConjugateTranspose());
}

bool UnitaryMatrix2x2::isUnitaryMatrix(matrix2x2 matrix) {
    return isUnitaryMatrix(UnitaryMatrix2x2(matrix));
}

bool UnitaryMatrix2x2::fuzzyCompare(double a, double b) { return qAbs(a - b) <= EPSILON * 10; }

void UnitaryMatrix2x2::print(std::ostream &out) const {
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
        cmp = true;
        complex ephi(1, 0);
        if (abs(op2.a() + op2.b()) < EPSILON) {
            ephi = (op2.a() - op2.b()) / (op1.a() - op1.b());
        } else {
            ephi = (op2.a() + op2.b()) / (op1.a() + op1.b());
        }

        complex a = op2.a() / ephi;
        complex b = op2.b() / ephi;
        complex c = op2.c() / ephi;
        complex d = op2.d() / ephi;

        cmp &= fuzzyCompare(op1.a().real(), a.real());
        cmp &= fuzzyCompare(op1.a().imag(), a.imag());

        cmp &= fuzzyCompare(op1.b().real(), b.real());
        cmp &= fuzzyCompare(op1.b().imag(), b.imag());

        cmp &= fuzzyCompare(op1.c().real(), c.real());
        cmp &= fuzzyCompare(op1.c().imag(), c.imag());

        cmp &= fuzzyCompare(op1.d().real(), d.real());
        cmp &= fuzzyCompare(op1.d().imag(), d.imag());
    }

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
    return UnitaryMatrix2x2({conj(_matrix.a), conj(_matrix.c), conj(_matrix.b), conj(_matrix.d)});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::operator*(const UnitaryMatrix2x2 &op) {
    complex a = _matrix.a * op.a() + _matrix.b * op.c();
    complex b = _matrix.a * op.b() + _matrix.b * op.d();
    complex c = _matrix.c * op.a() + _matrix.d * op.c();
    complex d = _matrix.c * op.b() + _matrix.d * op.d();
    return UnitaryMatrix2x2({a, b, c, d});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getId() { return UnitaryMatrix2x2({1, 0, 0, 1}); }
UnitaryMatrix2x2 UnitaryMatrix2x2::getX() { return UnitaryMatrix2x2({0, 1, 1, 0}); }
UnitaryMatrix2x2 UnitaryMatrix2x2::getY() {
    return UnitaryMatrix2x2({0, -complex(0, 1), complex(0, 1), 0});
}
UnitaryMatrix2x2 UnitaryMatrix2x2::getZ() { return UnitaryMatrix2x2({1, 0, 0, -1}); }
UnitaryMatrix2x2 UnitaryMatrix2x2::getH() {
    double h = 1 / sqrt(2);
    return UnitaryMatrix2x2({h, h, h, -h});
}
UnitaryMatrix2x2 UnitaryMatrix2x2::getS() { return UnitaryMatrix2x2({1, 0, 0, complex(0, 1)}); }
UnitaryMatrix2x2 UnitaryMatrix2x2::getT() {
    return UnitaryMatrix2x2({1, 0, 0, exp(complex(0, 1) * (M_PI / 4))});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getPhi(double gamma) {
    return UnitaryMatrix2x2({1, 0, 0, pow(C_E, C_I * gamma)});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getXrotate(double the) {
    the /= 2.0;
    return UnitaryMatrix2x2({cos(the), -C_I * sin(the), -C_I * sin(the), cos(the)});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getYrotate(double the) {
    the /= 2.0;
    return UnitaryMatrix2x2({cos(the), -sin(the), sin(the), cos(the)});
}

UnitaryMatrix2x2 UnitaryMatrix2x2::getZrotate(double the) {
    the /= 2.0;
    return UnitaryMatrix2x2({exp(-C_I * the), 0.0, 0.0, exp(C_I * the)});
}
