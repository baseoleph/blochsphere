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

#ifndef QUBIT_H
#define QUBIT_H

#include "point.h"
#include <complex>

typedef std::complex<double> complex;

class Qubit : public Point {
public:
    Qubit();
    Qubit(double x, double y, double z);
    Qubit(double the, double phi);
    Qubit(complex a, complex b);

    inline complex a() const { return a_; }
    inline complex b() const { return b_; }

protected:
    void changeQubit(double x, double y, double z);
    void changeQubit(double the, double phi);
    void changeQubit(complex a, complex b);

private:
    using Point::changePoint;

    complex a_;
    complex b_;

    void evalVertex();
    void evalAB();
};

#endif // QUBIT_H
