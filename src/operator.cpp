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

#include "operator.h"

Operator::Operator() {
}

QVector<Spike> Operator::rotate(Spike s, QVector3D v, double gamma) {
    QVector<Spike> trace = {s};
    for (size_t i = 1; i < DURATION; ++i) {
        QQuaternion q = QQuaternion::fromAxisAndAngle(v, i / DURATION * gamma);
        trace.append(Vector::actOperator(q, s));
    }
    QQuaternion q = QQuaternion::fromAxisAndAngle(v, gamma);
    trace.append(Vector::actOperator(q, s));
    return trace;
}

QVector<Spike> Operator::rXRotate(Spike s, double gamma) {
    return rotate(s, QVector3D(1, 0, 0), gamma);
}

QVector<Spike> Operator::rYRotate(Spike s, double gamma) {
    return rotate(s, QVector3D(0, 1, 0), gamma);
}

QVector<Spike> Operator::rZRotate(Spike s, double gamma) {
    return rotate(s, QVector3D(0, 0, 1), gamma);
}

QVector<Spike> Operator::applyZXDecomposition(Spike s, complex a, complex b, complex c, complex d) {
    QVector<Spike> spike;
    decomposition  dec = zxDecomposition(a, b, c, d);
    qDebug() << "alpha" << dec.alpha;
    qDebug() << "beta" << dec.beta;
    qDebug() << "gamma" << dec.gamma;
    qDebug() << "delta" << dec.delta;
    spike.append(rZRotate(s, dec.beta));
    spike.append(rXRotate(spike.last(), dec.gamma));
    spike.append(rZRotate(spike.last(), dec.delta));

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyOperator(Spike s, complex a, complex b, complex c, complex d) {
    QVector<Spike> spike = {s};
    decomposition  dec = zxDecomposition(a, b, c, d);
    QQuaternion    qz1 = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), dec.beta);
    QQuaternion    qx = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), dec.gamma);
    QQuaternion    qz2 = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), dec.delta);
    QQuaternion    q = qz1 * qx * qz2;

    // TODO check algorithm. q.scalar < 0; q.scalar < eps + and -
    QVector3D v = q.vector().normalized();
    double    g = qFuzzyIsNull(v.length()) ? 0 : 2 * qAcos(q.scalar()) * 180 / M_PI;
    if (qFuzzyIsNull(g)) {
        // TODO is this correct?
        g = g > 0 ? 180 : -180;
    }

    if (not qFuzzyIsNull(g)) {
        for (int i = 1; i < DURATION; ++i) {
            QQuaternion qq = QQuaternion::fromAxisAndAngle(v, i / DURATION * g);
            spike.append(Vector::actOperator(qq, s));
        }
    }
    spike.append(Vector::actOperator(q, s));

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QString getComplexStr(complex a) {
    double real = abs(a.real()) < 1e-6 ? 0 : a.real();
    double imag = abs(a.imag()) < 1e-6 ? 0 : a.imag();

    QString str = "(" + QString::number(real) + ", " + QString::number(imag) + ")";
    return str;
}

decomposition Operator::zxDecomposition(complex a, complex b, complex c, complex d) {
    //  Author   : Швецкий Михаил Владимирович
    //  Original: ff22dd04382d02a839c56f27a80ad18064eb6595

    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;

    if (abs(a) > EPSILON && abs(b) > EPSILON) {
        alpha = 0.5 * arg(a * d - b * c);
        beta = 0.5 * (arg(d / a) + arg(c / b));
        delta = 0.5 * (arg(d / a) - arg(c / b));
        complex z = exp(-i * (M_PI / 2.0 + 2.0 * alpha - beta));
        if (real(exp(-i * 2.0 * alpha) * (a * d + b * c)) > 0)
            gamma = real(asin(-2.0 * a * b * z));
        else
            gamma = M_PI - real(asin(-2.0 * a * b * z));
    } else if (abs(b) < EPSILON && abs(c) < EPSILON) {
        alpha = arg(a * d) / 2.0;
        gamma = 0;
        delta = 0; // delta - любое из R
        beta = -delta + arg(d / a);
    } else if (abs(a) < EPSILON && abs(d) < EPSILON) {
        alpha = arg(-b * c) / 2.0;
        beta = 0; // beta - любое из R
        delta = beta + arg(b / c);
        complex z = exp(-i * (M_PI / 2.0 + alpha - beta / 2.0 + delta / 2.0));
        if (real(-b * z) > 0)
            gamma = M_PI;
        else
            gamma = -M_PI;
    }
    double v = gamma / 2.0;
    if (abs(exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v) - a) > EPSILON &&
        abs(-i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v) - b) > EPSILON &&
        abs(-i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v) - c) > EPSILON &&
        abs(exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v) - d) > EPSILON) {
        alpha = M_PI + alpha;
    }

    complex aa = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    complex bb = -i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    complex cc = -i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    complex dd = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    qDebug() << getComplexStr(aa) << getComplexStr(bb);
    qDebug() << getComplexStr(cc) << getComplexStr(dd);

    return decomposition{alpha * (180 / M_PI), beta * (180 / M_PI), delta * (180 / M_PI),
                         gamma * (180 / M_PI)};
}
