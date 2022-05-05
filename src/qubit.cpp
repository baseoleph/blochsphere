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

#include "qubit.hpp"
#include <QtCore>
#include <complex>
#include <iostream>

Qubit::Qubit() { evalAB(); }
Qubit::Qubit(double x, double y, double z) : Point(x, y, z) { evalAB(); }
Qubit::Qubit(double the, double phi) : Point(the, phi) { evalAB(); }
Qubit::Qubit(complex a, complex b) : a_(a), b_(b) { evalVertex(); }

void Qubit::changeQubit(double x, double y, double z) {
    changePoint(x, y, z);
    evalAB();
}

void Qubit::changeQubit(double the, double phi) {
    changePoint(the, phi);
    evalAB();
}

void Qubit::changeQubit(complex a, complex b) {
    a_ = a;
    b_ = b;
    evalVertex();
}

void Qubit::evalVertex() {
    double the = 0;
    double phi = 0;

    if (abs(b_) > EPSILON) {
        phi = (complex(0, -1) * log(b_ / sqrt(complex(1, 0) - a_ * a_))).real();
    }

    if (abs(a_) == 0) {
        the = M_PI;
    } else {
        the = 2.0 * asin(abs(b_));
    }

    changePoint(the, phi);
}

void Qubit::evalAB() {
    complex csin(sin(the() / 2.0), 0.0);
    a_ = cos(the() / 2.0);
    b_ = exp(C_I * phi()) * csin;
}

void Qubit::printQubit() {
    std::cout << "---------------\n";
    std::cout << "xyz " << x() << " " << y() << " " << z() << "\n";
    std::cout << "pt " << qRadiansToDegrees(the()) << " " << qRadiansToDegrees(phi()) << "\n";
    std::cout << "ab (" << a().real() << ", " << a().imag() << "), (" << b().real() << ", "
              << b().imag() << ") \n";
    std::cout << "---------------\n";
}
