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

#include "utility.h"

#include <QCoreApplication>
#include <QRegExp>
#include <QTime>

namespace {
int speed = 5;
QRegExpValidator
    cmpVld(QRegExp(QString::fromUtf8("^[+-]?[0-9]*\\.?[0-9]*[+-]?[0-9]*\\.?[0-9]*[iIшШ]?$")));
QRegExpValidator axsVld(QRegExp("^-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*;?-?[\\d]*\\.?[\\d]*$"));
} // namespace

namespace Utility {
const QValidator *compValid() { return &cmpVld; }
const QValidator *axisValid() { return &axsVld; }

double getDuration() { return DURATION * (11 - speed); }

double roundNumber(double a, double s) {
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

    if (str.contains(rxp3)) {
        if (rxp3.capturedTexts()[1] == "+" || rxp3.capturedTexts()[1] == "") {
            return {0.0, 1.0};
        } else if (rxp3.capturedTexts()[1] == "-") {
            return {0.0, -1.0};
        } else {
            return {0.0, rxp3.capturedTexts()[1].toDouble()};
        }
    } else if (str.contains(rxp1)) {
        if (rxp1.capturedTexts()[2] == "+") {
            return {rxp1.capturedTexts()[1].toDouble(), 1.0};
        } else if (rxp1.capturedTexts()[2] == "-") {
            return {rxp1.capturedTexts()[1].toDouble(), -1.0};
        } else if (rxp1.capturedTexts().size() > 2) {
            return {rxp1.capturedTexts()[1].toDouble(), rxp1.capturedTexts()[2].toDouble()};
        } else {
            return {0, rxp1.capturedTexts()[1].toDouble()};
        }
    } else if (str.contains(rxp2)) {
        return {rxp2.capturedTexts()[1].toDouble(), 0.0};
    }

    return {0, 0};
}

QString parseComplexToStr(complex c, int d) {
    QString str;
    double  im = roundNumber(imag(c), d);
    double  re = roundNumber(real(c), d);

    if (not fuzzyCompare(re, 0.)) {
        str += QString::number(re);
    }

    if (not fuzzyCompare(im, 0.)) {
        if (fuzzyCompare(std::abs(im), 1.)) {
            str += im > 0 ? "+i" : "-i";
        } else {
            str += im >= 0 ? "+" : "";
            str += QString::number(im) + "i";
        }
    }

    if (str.size() == 0) {
        str = "0";
    } else if (str[0] == '+') {
        str = str.remove(0, 1);
    }

    return str;
}

QString numberToStr(double d) { return QString::number(roundNumber(d)); }
QString numberToStr(long d) { return QString::number(d); }
double  getSpeed() { return speed; }
void    setSpeed(int spd) { speed = spd; }

bool fuzzyCompare(double a, double b) { return qAbs(a - b) <= EPSILON * 10; }
bool fuzzyCompare(complex a, complex b) {
    return fuzzyCompare(a.real(), b.real()) && fuzzyCompare(a.imag(), b.imag());
}

void updateComplexLineEdit(QLineEdit *lineEdit) {
    QRegExp re(QString::fromUtf8("[IШш]"));
    lineEdit->setText(lineEdit->text().replace(re, "i"));
}

int random(int min, int max) { return min + rand() % ((max + 1) - min); }

double random(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void delay() {
    QTime dieTime = QTime::currentTime().addMSecs(50);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

} // namespace Utility
