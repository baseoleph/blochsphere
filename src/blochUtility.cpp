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

#include "blochUtility.h"

#include <QRegExp>
#include <QStringList>

double roundNumber(double a, double s = 1 / EPSILON) {
    a *= s;
    if (a - floor(a) >= 0.5)
        return (floor(a) + 1) / s;
    else
        return floor(a) / s;
}

complex parseStrToComplex(const QString &str) {
    QRegExp rxp1("^([+-]?[0-9]+\\.?[0-9]*)([+-]?[0-9]*\\.?[0-9]*)i$");
    QRegExp rxp2("^([+-]?[\\d]+\\.?[\\d]*)$");
    QRegExp rxp3("^([+-]?[0-9]*\\.?[0-9]*)i$");
    QRegExp rxp4("^([+-])i$");
    if (str.contains(rxp1)) {
        if (rxp1.capturedTexts()[2] == "+")
            return complex(rxp1.capturedTexts()[1].toDouble(), 1.0);
        else if (rxp1.capturedTexts()[2] == "-")
            return complex(rxp1.capturedTexts()[1].toDouble(), -1.0);
        else
            return complex(rxp1.capturedTexts()[1].toDouble(), rxp1.capturedTexts()[2].toDouble());
    } else if (str.contains(rxp2))
        return complex(rxp2.capturedTexts()[1].toDouble(), 0.0);
    else if (str.contains(rxp3)) {
        if (rxp3.capturedTexts()[1] == "+" || rxp3.capturedTexts()[1] == "")
            return complex(0.0, 1.0);
        else if (rxp3.capturedTexts()[1] == "-")
            return complex(0.0, -1.0);
        else
            return complex(0.0, rxp3.capturedTexts()[1].toDouble());
    }
    // DOTO nothing try to catch it
    throw 1;
}

QString parseComplexToStr(complex c, int d) {
    QString str;
    double  im = roundNumber(imag(c), d);
    double  re = roundNumber(real(c), d);
    if (abs(re) < EPSILON && abs(im) > EPSILON) {
        if (abs(1.0 - im) < EPSILON)
            return str += "i";
        else if (abs(1.0 + im) < EPSILON)
            return str += "-i";
        else
            return str += QString("%1i").arg(im);
    } else {
        str = QString("%1").arg(re);
        if (abs(im) > EPSILON) {
            if (im > EPSILON) {
                str += "+";
                if (1.0 - im < EPSILON)
                    return str += "i";
            }
            if (im < EPSILON) {
                if (1.0 + im < EPSILON)
                    return str += "-i";
            }
            str += QString("%1i").arg(im);
        }
        return str;
    }
    // DOTO nothing try to catch it
    throw 1;
}
QString numberToStr(double d) { return QString::number(roundNumber(d)); }
QString numberToStr(long d) { return QString::number(d); }
