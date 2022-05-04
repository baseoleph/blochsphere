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

#ifndef QUBITS_H
#define QUBITS_H

#include "qubit.hpp"

struct QubitFields {
    complex a;
    complex b;
    double  the;
    double  phi;
    double  x;
    double  y;
    double  z;
    bool    aCheck = true;
    bool    bCheck = true;
    bool    tCheck = true;
    bool    pCheck = true;
    bool    xCheck = true;
    bool    yCheck = true;
    bool    zCheck = true;
};

QVector<QubitFields> qubits();
QubitFields          getQubitFields(const Qubit &q);

#endif // QUBITS_H
