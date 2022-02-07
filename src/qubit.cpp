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

#include "qubit.h"
#include <complex>

Qubit::Qubit() {
    x_ = 0;
    y_ = 0;
    z_ = 0;
    evalPT();
    evalAB();
}

// TODO why I cant use x_(x)?
Qubit::Qubit(double x, double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
    evalPT();
    evalAB();
}

Qubit::Qubit(double the, double phi) {
    the_ = the;
    phi_ = phi;
    evalXYZ();
    evalAB();
}

Qubit::Qubit(complex a, complex b) : a_(a), b_(b) {
    evalVertex();
}

void Qubit::evalVertex() {
    // TODO must be tested
    phi_ = arg(b_) - M_PI;
    if (abs(a_) == 0) {
        the_ = M_PI;
    } else {
        the_ = 2.0 * asin(abs(b_));
    }
    evalXYZ();
}

void Qubit::evalAB() {
    const complex C_E = complex(exp(1), 0.0);
    const complex C_I = complex(0.0, 1.0);
    complex       csin(sin(the_ / 2.0), 0.0);
    a_ = cos(the_ / 2.0);
    b_ = pow(C_E, C_I * phi_) * csin;
}
