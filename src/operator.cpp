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
    QVector<Spike> trace;
    trace.append(s);
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

    if (dec.delta != 0) {
        QVector<Spike> vct = rZRotate(spike.last(), dec.delta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.gamma != 0) {
        QVector<Spike> vct = rXRotate(spike.last(), dec.gamma);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.beta != 0) {
        QVector<Spike> vct = rZRotate(spike.last(), dec.beta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyZYDecomposition(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = zyDecomposition(op);
    if (dec.delta != 0) {
        QVector<Spike> vct = rZRotate(spike.last(), dec.delta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.gamma != 0) {
        QVector<Spike> vct = rYRotate(spike.last(), dec.gamma);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.beta != 0) {
        QVector<Spike> vct = rZRotate(spike.last(), dec.beta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyXYDecomposition(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = xyDecomposition(op);

    if (dec.delta != 0) {
        QVector<Spike> vct = rXRotate(spike.last(), dec.delta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.gamma != 0) {
        QVector<Spike> vct = rYRotate(spike.last(), dec.gamma);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    if (dec.beta != 0) {
        QVector<Spike> vct = rXRotate(spike.last(), dec.beta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyOperator(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = zyDecomposition(op);
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

    decomposition dec;
    dec.alpha = alpha * (180 / M_PI);
    dec.beta = beta * (180 / M_PI);
    dec.delta = delta * (180 / M_PI);
    dec.gamma = gamma * (180 / M_PI);
    return dec;
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
        if (real(exp(-i * 2.0 * alpha) * (a * d + b * c)) > 0) {
            gamma = real(asin(-2.0 * a * b * z));
        } else {
            gamma = M_PI - real(asin(-2.0 * a * b * z));
        }
    } else if (abs(b) < EPSILON && abs(c) < EPSILON) {
        alpha = arg(a * d) / 2.0;
        delta = 0;
        beta = -delta + arg(d / a);
        gamma = 0;
    } else if (abs(a) < EPSILON && abs(d) < EPSILON) {
        if (abs(1.0 - b * c) < EPSILON) {
            alpha = M_PI / 2.0;
        } else {
            alpha = arg(-b * c) / 2.0;
        }
        beta = 0;
        gamma = M_PI;
        delta = beta + arg(-b / c);
    }

    double v = gamma / 2.0;
    if ((abs(a - exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v)) > EPSILON) ||
        (abs(b + exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v)) > EPSILON)) {
        alpha = M_PI + alpha;
    }

    decomposition dec;
    dec.alpha = alpha * (180 / M_PI);
    dec.beta = beta * (180 / M_PI);
    dec.delta = delta * (180 / M_PI);
    dec.gamma = gamma * (180 / M_PI);
    return dec;
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

    decomposition dec;
    dec.alpha = alpha * (180 / M_PI);
    dec.beta = beta * (180 / M_PI);
    dec.delta = delta * (180 / M_PI);
    dec.gamma = gamma * (180 / M_PI);
    return dec;
}

decomposition Operator::xyDecomposition() { return xyDecomposition(_op); }

decomposition Operator::zyxDecomposition(UnitaryMatrix2x2 op) {
    //  Compiler: Borland C++ 3.1
    //  Author  : Швецкий Михаил Владимирович (28.12.2014,14:36-23:00;
    //                                         01.04-10.05.2018;
    //                                         07.06.2018,04:35)
    if (abs(d) > EPS)
        if (abs(1 + a / conj(d)) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(a / conj(d)) / 2.0;
    else if (abs(c) > EPS)
        if (abs(1 - b / conj(c)) < EPS)
            alpha = M_PI / 2.0;
        else
            alpha = arg(-b / conj(c)) / 2.0;
    // -----------------------------------
    complex A, B;
    A = (exp(-i * alpha) * (a + b) + exp(i * alpha) * (conj(c) + conj(d))) / 2.0;
    B = (exp(-i * alpha) * (a + b) - exp(i * alpha) * (conj(c) + conj(d))) / 2.0;
    cout << "Определитель полученной специальной матрицы: " << A * conj(A) + B * conj(B) << endl
         << endl;
    getch();
    double a1, a2, b1, b2;
    a1 = real(A);
    a2 = imag(A);
    b1 = real(B);
    b2 = imag(B);
    // ----------------------------------------------------
    cout << "Контрольный вывод a1+b1, a2-b2, a1-b1, a2+b2:" << endl
         << endl
         << a1 + b1 << ", " << a2 - b2 << ", " << a1 - b1 << ", " << a2 + b2 << endl
         << endl;
    // --------------------------------------------
    if (fabs(a1 + b1) < EPS && fabs(a2 + b2) < EPS && fabs(a2 - b2) > EPS) {
        cout << "Случай (1). "
             << "Внимание! Эффект Gimble lock, gamma=pi/2" << endl
             << endl;
        gamma = M_PI / 2.0;
        delta = 0; // delta - любое из R
        if (a1 - b1 > 0)
            beta = delta + 2.0 * asin(-1.0 / sqrt(2.0) * (a2 - b2));
        else
            beta = 2.0 * M_PI - delta - 2.0 * asin(-1.0 / sqrt(2.0) * (a2 - b2));
    } else {
        if (fabs(a1 + b1) < EPS && fabs(a2 + b2) < EPS && fabs(a1 - b1) > EPS) {
            cout << "Случай (2). "
                 << "Внимание! Эффект Gimble lock, gamma=pi/2" << endl
                 << endl;
            gamma = M_PI / 2.0;
            delta = 0; // delta - любое из R
            if (-a2 + b2 > 0)
                beta = delta + 2.0 * acos(1.0 / sqrt(2.0) * (a1 - b1));
            else
                beta = delta - 2.0 * acos(1.0 / sqrt(2.0) * (a1 - b1));
        } else
            ;
    }
    // ------------------------------------
    if (fabs(a2 - b2) < EPS && fabs(a1 - b1) < EPS)
        if (abs(a1 + b1) > EPS) {
            cout << "Случай (3). "
                 << "Внимание! Эффект Gimble lock, gamma=-pi/2" << endl
                 << endl;
            gamma = -M_PI / 2; // или gamma=3*M_PI/2;
            delta = 0;         // delta - любое из R
            beta = -delta - 2.0 * atan((a2 + b2) / (a1 + b1));
        } else if (fabs(a2 + b2) > EPS) {
            cout << "Случай (4). "
                 << "Внимание! Эффект Gimble lock, gamma=-pi/2" << endl
                 << endl;
            gamma = -M_PI / 2.0;
            delta = M_PI; // delta - любое из R
            beta = 2.0 * atan((a1 + b1) / (a2 + b2));
        } else
            ;
    // -----------------------------------
    if (fabs(a1 + b1) < EPS && fabs(a2 - b2) > EPS && fabs(a1 - b1) < EPS && fabs(a2 + b2) > EPS) {
        cout << "Случай (5)." << endl;
        beta = M_PI;
        delta = 0;
        if (-a2 > 0 || fabs(a2) < EPS)
            gamma = 2.0 * asin(b2);
        else
            gamma = 2.0 * M_PI - 2.0 * asin(b2);
    } else
        ;
    // -----------------------------------
    if (fabs(a1 + b1) > EPS && fabs(a2 - b2) > EPS && fabs(a1 - b1) < EPS && fabs(a2 + b2) < EPS) {
        cout << "Случай (6)." << endl;
        beta = M_PI / 2.0;
        delta = -M_PI / 2.0;
        if (a1 + b1 > 0)
            gamma = -M_PI / 2.0 + 2 * asin(sqrt(2.0) / 2.0 * (-a2 + b2));
        else
            gamma = 2.0 * M_PI - (-M_PI / 2.0 + 2 * asin(sqrt(2.0) / 2.0 * (-a2 + b2)));
        /*
          // -----------------------------
          // Вариант вычислений угла gamma
          // -----------------------------
          if (a1+b1-a2+b2>0)
            gamma=2.0*asin(-(a1+b1+a2-b2)/2.0);
          else gamma=2.0*M_PI-2.0*asin(-(a1+b1+a2-b2)/2.0);
        */
    } else
        ;
    // -----------------------------------
    if (fabs(a1 + b1) > EPS && fabs(a2 - b2) < EPS && fabs(a1 - b1) > EPS && fabs(a2 + b2) < EPS) {
        cout << "Случай (7)." << endl;
        beta = 0;
        delta = 0;
        // ---------------------------------------------
        // Решение системы тригонометрических уравнений:
        //
        //  sin(gamma/2)=-b1;
        //  cos(gamma/2)= a1
        // ----------------------
        if (a1 > 0 || fabs(a1) < EPS)
            gamma = 2.0 * asin(-b1);
        else
            gamma = 2.0 * M_PI - 2.0 * asin(-b1);
    } else
        ;
    // -------------------------------------------------------
    if (fabs(a1 + b1) > EPS && fabs(a1 - b1) > EPS && fabs(a2 + b2) > EPS)
    // && fabs(a2-b2)>EPS)
    {
        cout << "Случай (8)." << endl;
        beta = -atan((a2 + b2) / (a1 + b1)) - atan((a2 - b2) / (a1 - b1));
        delta = -atan((a2 + b2) / (a1 + b1)) + atan((a2 - b2) / (a1 - b1));

        if (sqrt(2.0) / 2.0 * (a1 - b1) / cos(beta / 2.0 - delta / 2.0) > 0)
            gamma = M_PI / 2.0 +
                    2.0 * asin(sqrt(2.0) / 2.0 * (a2 + b2) / sin(beta / 2.0 + delta / 2.0));
        else {
            gamma = M_PI / 2.0 -
                    2.0 * asin(sqrt(2.0) / 2.0 * (a2 + b2) / sin(beta / 2.0 + delta / 2.0));
            alpha = alpha + M_PI;
        }
    } else
        ;
    // clang-format on
}

decomposition Operator::zyxDecomposition() { return zyxDecomposition(_op); }

vectorangle Operator::vectorAngleDec(UnitaryMatrix2x2 op) {
    decomposition zyDec = Operator::zyDecomposition(op);

    zyDec.gamma = zyDec.gamma / 180 * M_PI;
    zyDec.beta = zyDec.beta / 180 * M_PI;
    zyDec.delta = zyDec.delta / 180 * M_PI;

    vectorangle va;

    va.angle = 2.0 * acos(cos(zyDec.gamma / 2.0) * cos(zyDec.beta / 2.0 + zyDec.delta / 2.0));
    double sint = sin(va.angle / 2.0);
    if (abs(sint) > EPSILON) {
        va.x = sin(zyDec.gamma / 2.0) / sint * sin(zyDec.delta / 2.0 - zyDec.beta / 2.0);
        va.y = sin(zyDec.gamma / 2.0) / sint * cos(zyDec.beta / 2.0 - zyDec.delta / 2.0);
        va.z = cos(zyDec.gamma / 2.0) / sint * sin(zyDec.beta / 2.0 + zyDec.delta / 2.0);
    } else {
        va.x = 0;
        va.y = 0;
        va.z = 0;
    }

    return va;
}

vectorangle Operator::vectorAngleDec() { return vectorAngleDec(_op); }

UnitaryMatrix2x2 Operator::genRandUnitaryMatrix(qint64 seed) {
    UnitaryMatrix2x2 op;
    matrix2x2        matrix;

    // DOTO algorithm mast work every time!!
    // DOTO get rid of c-style rand, must use c++-style
    complex i{0, 1};
    double  a1, a2, b1, b2, moda, modb, phi;
    srand(time(nullptr) * rand());
    moda = rand() / double(RAND_MAX);
    a1 = random(0., 1.);
    a2 = random(0., 1. - a1 * a1);
    b1 = random(0., 1. - a1 * a1 - a2 * a2);
    b2 = sqrt(1 - a1 * a1 - a2 * a2 - b1 * b1);

    // TODO check with and without
    //    phi = floor((2.0 * M_PI * rand() / double(RAND_MAX) - M_PI) * 180.0 / M_PI);
    phi = rand();

    matrix.a = exp(i * phi) * complex(a1, a2);
    matrix.b = exp(i * phi) * complex(b1, b2);
    matrix.c = -exp(i * phi) * conj(matrix.b);
    matrix.d = exp(i * phi) * conj(matrix.a);

    //    qDebug() << a1 << a2;
    //    qDebug() << b1 << b2;

    // TODO check for exception
    op.updateMatrix(matrix);
    return op;
}

void Operator::setOperator(UnitaryMatrix2x2 op) { _op = op; }

QVector<Spike> Operator::applyOperator(Spike s) { return applyOperator(s, _op); }

QVector<Spike> Operator::applyVectorRotation(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    vectorangle    va = vectorAngleDec(op);

    QVector<Spike> vct = rotate(s, QVector3D(va.x, va.y, va.z), va.angle * 180 / M_PI);
    for (auto &e : vct) {
        spike.append(e);
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyVectorRotation(Spike s) { return applyVectorRotation(s, _op); }

void Operator::toX() { _op = UnitaryMatrix2x2::getX(); }
void Operator::toY() { _op = UnitaryMatrix2x2::getY(); }
void Operator::toZ() { _op = UnitaryMatrix2x2::getZ(); }
void Operator::toH() { _op = UnitaryMatrix2x2::getH(); }
void Operator::toS() { _op = UnitaryMatrix2x2::getS(); }
void Operator::toT() { _op = UnitaryMatrix2x2::getT(); }
void Operator::toPhi(double gamma) { _op = UnitaryMatrix2x2::getPhi(gamma); }
void Operator::toXrotate(double the) { _op = UnitaryMatrix2x2::getXrotate(the); }
void Operator::toYrotate(double the) { _op = UnitaryMatrix2x2::getYrotate(the); }
void Operator::toZrotate(double the) { _op = UnitaryMatrix2x2::getZrotate(the); }

QVector<Spike> Operator::applyZXDecomposition(Spike s) { return applyZXDecomposition(s, _op); }
QVector<Spike> Operator::applyZYDecomposition(Spike s) { return applyZYDecomposition(s, _op); }
QVector<Spike> Operator::applyXYDecomposition(Spike s) { return applyXYDecomposition(s, _op); }

bool Operator::setOperatorByZXDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByZxDec(dec))) {
        return false;
    }
    setOperator(matrixOp);
    return true;
}

bool Operator::setOperatorByZYDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByZyDec(dec))) {
        return false;
    }
    setOperator(matrixOp);
    return true;
}

bool Operator::setOperatorByXYDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByXyDec(dec))) {
        return false;
    }
    setOperator(matrixOp);
    return true;
}

bool Operator::setOperatorByVectorAngle(vectorangle va) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByVecAng(va))) {
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

matrix2x2 Operator::getMatrixByVecAng(vectorangle va) {
    double    v = va.angle / 2;
    matrix2x2 matrix;
    complex   i{0, 1};

    matrix.a = cos(v) - i * sin(v) * va.z;
    matrix.b = -i * sin(v) * (va.x - i * va.y);
    matrix.c = -i * sin(v) * (va.x + i * va.y);
    matrix.d = cos(v) + i * sin(v) * va.z;
    matrix.a *= i;
    matrix.b *= i;
    matrix.c *= i;
    matrix.d *= i;
    return matrix;
}

void Operator::toRnRotate(vectorangle) {
    //        curOperator = cos(ng / 2.0) * Iop - C_I * sin(ng / 2.0) * (nX * Xop + nY * Yop + nZ *
    //        Zop);
}
