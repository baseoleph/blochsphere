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

#include <QDebug>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QString>
#include <complex>

#define EPSILON 0.000001
#define C_I complex(0, 1)
#define M_PI 3.14159265358979323846
#define DURATION 100.
#define MAX_COUNT_SPHERES 5
#define MAX_COUNT_OF_STEPS 100
#define BLOCHSPHERE_VERSION "v1.0.2"

typedef std::complex<double> complex;

namespace Utility {
const QValidator *compValid();
const QValidator *axisValid();

complex parseStrToComplex(const QString &str);
QString parseComplexToStr(complex c, int d = 1 / EPSILON);
bool    fuzzyCompare(double a, double b);
bool    fuzzyCompare(complex a, complex b);
QString numberToStr(double d);
QString numberToStr(long d);
double  roundNumber(double a, double s = 1 / EPSILON);
double  getDuration();
double  getSpeed();
void    setSpeed(int spd);
void    updateComplexLineEdit(QLineEdit *lineEdit);

int    random(int min, int max);
double random(double fMin, double fMax);

void delay();
} // namespace Utility
#endif // BLOCHUTILITY_H
