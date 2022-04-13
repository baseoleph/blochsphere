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

#ifndef BLOCHUTILITY_H
#define BLOCHUTILITY_H

#include <QString>
#include <complex>

#define EEE 2.71828182845904523536
#define EPSILON 0.000001
#define C_I complex(0, 1)
#define C_E complex(EEE, 0.0)
#define M_PI 3.14159265358979323846
// TODO change logic duration of animation
#define DURATION 1000.

typedef std::complex<double> complex;

// DOTO improve functions; increase accuracy
complex parseStrToComplex(const QString &str);
QString parseComplexToStr(complex c, int d = 1 / EPSILON);
QString numberToStr(double d);
QString numberToStr(long d);
double  roundNumber(double a, double s);

// TODO must be double
inline int random(int min, int max) {
    return min + rand() % ((max + 1) - min);
}

#endif // BLOCHUTILITY_H
