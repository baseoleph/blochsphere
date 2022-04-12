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

#include "operator.hpp"

Operator::Operator() {}

QVector<Spike> Operator::rotate(Spike s, QVector3D v, double gamma) {
    QVector<Spike> trace = {s};
    for (uint i = 1; i < DURATION; ++i) {
        QQuaternion q = QQuaternion::fromAxisAndAngle(v, static_cast<float>(i / DURATION * gamma));
        trace.append(Vector::actOperator(q, s));
    }
    QQuaternion q = QQuaternion::fromAxisAndAngle(v, static_cast<float>(gamma));
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

QVector<Spike> Operator::applyZXDecomposition(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = zxDecomposition(op);
    if (dec.beta != 0) {
        spike.append(rZRotate(spike.last(), dec.beta));
    }
    if (dec.gamma != 0) {
        spike.append(rXRotate(spike.last(), dec.gamma));
    }
    if (dec.delta != 0) {
        spike.append(rZRotate(spike.last(), dec.delta));
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyOperator(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = zxDecomposition(op);
    QVector3D      zVector = QVector3D(0, 0, 1);
    QVector3D      xVector = QVector3D(1, 0, 0);
    QQuaternion    qz1 = QQuaternion::fromAxisAndAngle(zVector, static_cast<float>(dec.beta));
    QQuaternion    qx = QQuaternion::fromAxisAndAngle(xVector, static_cast<float>(dec.gamma));
    QQuaternion    qz2 = QQuaternion::fromAxisAndAngle(zVector, static_cast<float>(dec.delta));
    QQuaternion    q = qz1 * qx * qz2;

    // TODO check algorithm. q.scalar < 0; q.scalar < EPSILON + and -
    QVector3D v = q.vector().normalized();
    double    g = qFuzzyIsNull(v.length()) ? 0 : 2 * qAcos(q.scalar()) * 180 / M_PI;
    if (qFuzzyIsNull(g)) {
        // TODO is this correct?
        g = g > 0 ? 180 : -180;
    }

    if (not qFuzzyIsNull(g)) {
        for (uint i = 1; i < DURATION; ++i) {
            QQuaternion qq = QQuaternion::fromAxisAndAngle(v, static_cast<float>(i / DURATION * g));
            spike.append(Vector::actOperator(qq, s));
        }
    }
    spike.append(Vector::actOperator(q, s));

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QString getComplexStr(complex a) {
    double real = qFuzzyIsNull(a.real()) ? 0 : a.real();
    double imag = qFuzzyIsNull(a.imag()) ? 0 : a.imag();

    QString str = "(" + QString::number(real) + ", " + QString::number(imag) + ")";
    return str;
}

decomposition Operator::zxDecomposition(UnitaryMatrix2x2 op) {
    //  Author  : Швецкий Михаил Владимирович
    //  Original: ff22dd04382d02a839c56f27a80ad18064eb6595

    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();

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
        delta = 0;
        beta = -delta + arg(d / a);
    } else if (abs(a) < EPSILON && abs(d) < EPSILON) {
        alpha = arg(-b * c) / 2.0;
        beta = 0;
        delta = beta + arg(b / c);
        complex z = exp(-i * (M_PI / 2.0 + alpha - beta / 2.0 + delta / 2.0));
        if (real(-b * z) > 0)
            gamma = M_PI;
        else
            gamma = -M_PI;
    }
    double v = gamma / 2.0;

    // TODO there is need tests for this if
    if (abs(exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v) - a) > EPSILON ||
        abs(-i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v) - b) > EPSILON ||
        abs(-i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v) - c) > EPSILON ||
        abs(exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v) - d) > EPSILON) {
        alpha = M_PI + alpha;
    }

    //    complex aa = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    //    complex bb = -i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    //    complex cc = -i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    //    complex dd = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    //    qDebug() << getComplexStr(aa) << getComplexStr(bb);
    //    qDebug() << getComplexStr(cc) << getComplexStr(dd);

    return decomposition{alpha * (180 / M_PI), beta * (180 / M_PI), delta * (180 / M_PI),
                         gamma * (180 / M_PI)};
}

decomposition Operator::zxDecomposition() { return zxDecomposition(_op); }

decomposition Operator::zyDecomposition(UnitaryMatrix2x2 op) {
    //  Author  : Швецкий Михаил Владимирович
    //  Original: 690dca3e8dcc32e4b4eed0fcfc65c5bdf8dd06b0

    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();

    if (abs(a) > EPSILON && abs(b) > EPSILON) {
        alpha = arg(a * d - b * c) / 2.0;
        beta = (arg(d / a) + arg(-c / b)) / 2.0;
        delta = (arg(d / a) - arg(-c / b)) / 2.0;

        complex z = exp(-i * (2.0 * alpha - beta));
        if (real(exp(-i * 2.0 * alpha) * (a * d + b * c)) > 0)
            gamma = real(asin(-2.0 * a * b * z));
        else
            gamma = M_PI - real(asin(-2.0 * a * b * z));
    } else if (abs(b) < EPSILON && abs(c) < EPSILON) {
        alpha = arg(a * d) / 2.0;
        delta = 0;
        beta = -delta + arg(d / a);
        gamma = 0;
    } else if (abs(a) < EPSILON && abs(d) < EPSILON) {
        alpha = arg(-b * c) / 2.0;
        beta = 0;
        delta = beta + arg(-b / c);
        if (real(c * exp(-i * (alpha + beta / 2.0 - delta / 2.0))) > 0)
            gamma = M_PI;
        else
            gamma = -M_PI;
    }

    double v = gamma / 2.0;
    if ((abs(a - exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v)) > EPSILON) ||
        (abs(b + exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v)) > EPSILON)) {
        alpha = M_PI + alpha;
    }

    return decomposition{alpha * (180 / M_PI), beta * (180 / M_PI), delta * (180 / M_PI),
                         gamma * (180 / M_PI)};
}

decomposition Operator::zyDecomposition() { return zyDecomposition(_op); }

decomposition Operator::xyDecomposition(UnitaryMatrix2x2 op) {
    //  Author  : Швецкий Михаил Владимирович
    //  Original: cdc00f14787f28de44d8ae0ee1582ebaa82aba0e

    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();

    if (abs(d) > EPSILON)
        if (abs(complex(1) + a / conj(d)) < EPSILON)
            alpha = M_PI / 2.0;
        else
            alpha = arg(a / conj(d)) / 2.0;
    else if (abs(c) > EPSILON)
        if (abs(complex(1) - b / conj(c)) < EPSILON)
            alpha = M_PI / 2.0;
        else
            alpha = arg(-b / conj(c)) / 2.0;

    complex A, B;
    double  a1, a2, b1, b2;
    A = (exp(-i * alpha) * (a + b) + exp(i * alpha) * (conj(c) + conj(d))) / 2.0;
    B = (exp(-i * alpha) * (a + b) - exp(i * alpha) * (conj(c) + conj(d))) / 2.0;

    a1 = real(A);
    a2 = imag(A);
    b1 = real(B);
    b2 = imag(B);

    if (fabs(a1) < EPSILON && fabs(b2) < EPSILON) // a1=0, b2=0
        if (fabs(b1) > EPSILON) {
            delta = 0; // delta - любое из R
            beta = delta + 2.0 * atan(a2 / b1);
            gamma = M_PI;
        } else if (fabs(a2) > EPSILON) {
            delta = 0; // delta - любое из R
            beta = delta + M_PI - 2.0 * atan(b1 / a2);
            gamma = M_PI;
        } else
            ;

    if (fabs(a2) < EPSILON && fabs(b1) < EPSILON) // a2=0, b1=0
        if (fabs(a1) > EPSILON) {
            delta = 0; // delta - любое из R
            if (a1 > 0)
                beta = -delta + 2.0 * asin(-b2);
            else
                beta = 2.0 * M_PI - (-delta + 2.0 * asin(-b2));
            gamma = 0;
        } else if (fabs(b2) > EPSILON) {
            delta = 0; // delta - любое из R
            beta = -delta + M_PI - 2.0 * atan(-a1 / b2);
            gamma = 0;
        } else
            ;

    if (fabs(a1) < EPSILON && fabs(a2) > EPSILON && fabs(b1) < EPSILON &&
        fabs(b2) > EPSILON) // a1=0, a2!=0, b1=0, b2!=0
    {
        beta = M_PI;
        gamma = 2.0 * asin(-a2);
        delta = 0;
    } else
        ;

    if (fabs(a1) > EPSILON && fabs(a2) > EPSILON && fabs(b1) < EPSILON &&
        fabs(b2) < EPSILON) // a1!=0, a2!=0, b1=0, b2=0
    {
        beta = M_PI / 2.0;
        if (a1 > 0)
            gamma = 2.0 * asin(-a2);
        else
            gamma = 2.0 * M_PI - 2.0 * asin(-a2);
        delta = -M_PI / 2.0;
    } else
        ;

    if (fabs(a1) > EPSILON && fabs(a2) < EPSILON && fabs(b1) > EPSILON &&
        fabs(b2) < EPSILON) // a1!=0, a2=0, b1!=0, b2=0
    {
        beta = 0;
        if (a1 > 0)
            gamma = -2.0 * asin(b1);
        else
            gamma = 2 * M_PI - (-2.0 * asin(b1));
        delta = 0;
    } else
        ;

    if (fabs(a1) > EPSILON && fabs(a2) < EPSILON && fabs(b1) > EPSILON &&
        fabs(b2) > EPSILON) // a1!=0, a2=0, b1!=0, b2!=0
    {
        gamma = 2.0 * asin(-b1);
        beta = asin(-b2 / cos(gamma / 2.0)); // beta=atan(-b2/a1);
        delta = beta;
    } else
        ;

    if (fabs(a1) > EPSILON && fabs(a2) > EPSILON && fabs(b2) > EPSILON) {
        beta = atan(-b2 / a1) + atan(a2 / b1);
        delta = atan(-b2 / a1) - atan(a2 / b1);
        if (a1 / cos(beta / 2.0 + delta / 2.0) > 0)
            gamma = 2.0 * asin(-a2 / sin(beta / 2.0 - delta / 2.0));
        else
            gamma = 2.0 * M_PI - 2.0 * asin(-a2 / sin(beta / 2.0 - delta / 2.0));
    } else
        ;

    return decomposition{alpha * (180 / M_PI), beta * (180 / M_PI), delta * (180 / M_PI),
                         gamma * (180 / M_PI)};
}

decomposition Operator::xyDecomposition() { return xyDecomposition(_op); }

UnitaryMatrix2x2 Operator::genRandUnitaryMatrix(qint64 seed) {
    UnitaryMatrix2x2   op;
    complex            i{0, 1};
    double             a1, a2, b1, b2, moda, modb, phi;
    QRandomGenerator64 rd(seed);
    moda = rd.generateDouble();
    a1 = rd.generateDouble() * sqrt(moda);
    a2 = sqrt(moda - a1 * a1);
    b1 = rd.generateDouble() * sqrt(modb = 1 - a1 * a1 - a2 * a2);
    b2 = sqrt(modb - b1 * b1);
    phi = floor((2.0 * M_PI * rd.generateDouble() - M_PI) * 180.0 / M_PI);
    complex a = exp(i * phi) * complex(a1, a2);
    complex b = exp(i * phi) * complex(b1, b2);
    complex c = -exp(i * phi) * conj(b);
    complex d = exp(i * phi) * conj(a);

    // TODO check for exception
    op.updateMatrix({a, b, c, d});
    return op;
}

void Operator::setOperator(UnitaryMatrix2x2 op) { _op = op; }

QVector<Spike> Operator::applyOperator(Spike s) { return applyOperator(s, _op); }

void Operator::toX() { _op = UnitaryMatrix2x2::getX(); }
void Operator::toY() { _op = UnitaryMatrix2x2::getY(); }
void Operator::toZ() { _op = UnitaryMatrix2x2::getZ(); }
void Operator::toH() { _op = UnitaryMatrix2x2::getH(); }
void Operator::toS() { _op = UnitaryMatrix2x2::getS(); }
void Operator::toT() { _op = UnitaryMatrix2x2::getT(); }

QVector<Spike> Operator::applyZXDecomposition(Spike s) { return applyZXDecomposition(s, _op); }
bool           Operator::setOperatorByZXDecomposition(decomposition dec) {
    // TODO use getMatrixBy functions
    double alpha = dec.alpha * M_PI / 180;
    double beta = dec.beta * M_PI / 180;
    double delta = dec.delta * M_PI / 180;
    double v = dec.gamma * M_PI / 360;

    complex i = complex(0, 1);
    complex a = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    complex b = -i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    complex c = -i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    complex d = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);

    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix({a, b, c, d})) {
        return false;
    }

    setOperator(matrixOp);
    return true;
}

void Operator::toId() { _op = UnitaryMatrix2x2(); }

matrix2x2 Operator::getMatrixByZxDec(decomposition dec) {
    double    alpha = dec.alpha * M_PI / 180;
    double    beta = dec.beta * M_PI / 180;
    double    delta = dec.delta * M_PI / 180;
    double    v = dec.gamma * M_PI / 360;
    matrix2x2 matrix;
    complex   i{0, 1};
    matrix.a = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    matrix.b = -i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    matrix.c = -i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    matrix.d = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    return matrix;
}

matrix2x2 Operator::getMatrixByZyDec(decomposition dec) {
    double    alpha = dec.alpha * M_PI / 180;
    double    beta = dec.beta * M_PI / 180;
    double    delta = dec.delta * M_PI / 180;
    double    v = dec.gamma * M_PI / 360;
    matrix2x2 matrix;
    complex   i{0, 1};
    matrix.a = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    matrix.b = -exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    matrix.c = exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    matrix.d = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    return matrix;
}

matrix2x2 Operator::getMatrixByXyDec(decomposition dec) {
    double    alpha = dec.alpha * M_PI / 180;
    double    beta = dec.beta * M_PI / 180;
    double    delta = dec.delta * M_PI / 180;
    double    v = dec.gamma * M_PI / 360;
    matrix2x2 matrix;
    complex   i{0, 1};
    double    w = beta / 2.0 + delta / 2.0;
    double    u = beta / 2.0 - delta / 2.0;

    matrix.a = exp(i * alpha) * (cos(v) * cos(w) - i * sin(v) * sin(u));
    matrix.b = exp(i * alpha) * (-sin(v) * cos(u) - i * cos(v) * sin(w));
    matrix.c = exp(i * alpha) * (sin(v) * cos(u) - i * cos(v) * sin(w));
    matrix.d = exp(i * alpha) * (cos(v) * cos(w) + i * sin(v) * sin(u));
    return matrix;
}
