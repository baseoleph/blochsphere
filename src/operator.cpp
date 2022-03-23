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

void Operator::zxDecomposition(complex a, complex b, complex c, complex d) {
    // clang-format off
    //
   //  Compiler: Borland C++ 3.1
   //  Autor   : Швецкий Михаил Владимирович (16.11.2014, 17:00-17:03;
   //                                         25.11.2014, 18:30-21:06;
   //                                         01.04-09.05.2018)

    if (abs(a)>EPS && abs(b)>EPS)
    {
        alpha=0.5*arg(a*d-b*c);
        beta =0.5*(arg(d/a)+arg(c/b));
        delta=0.5*(arg(d/a)-arg(c/b));
        // --------------------------------------------
        // Корректировка значения gamma, вычисленного с
        // помощью уравнения
        //
        //               -i*(pi/2+2*alpha-betta)
        //  sin(gamma)=-e                       *2*a*b
        //
        // в зависимости от знака cos(gamma), вычислен-
        // ного с помощью уравнения
        //
        //              -i*2*alpha
        //  cos(gamma)=e          (a*d+b*c)
        //
        // -----------------------------------------
        complex z=exp(-i*(M_PI/2.0+2.0*alpha-beta));
        if (real(exp(-i*2.0*alpha)*(a*d+b*c))>0)
            gamma=real(asin(-2.0*a*b*z));
        else gamma=M_PI-real(asin(-2.0*a*b*z));
    }
    else;
    // --------------------------
    if (abs(b)<EPS && abs(c)<EPS)
    {
        alpha=arg(a*d)/2.0;
        gamma=0;
        delta=0;                      // delta - любое из R
        beta =-delta+arg(d/a);
    }
    else;
    // --------------------------
    if (abs(a)<EPS && abs(d)<EPS)
    {
        alpha=arg(-b*c)/2.0;
        beta =0;                      // beta - любое из R
        delta=beta+arg(b/c);
        complex z=exp(-i*(M_PI/2.0+alpha-beta/2.0+delta/2.0));
        if (real(-b*z)>0)
            gamma=M_PI;
        else gamma=-M_PI;
    }
    else;
    // ------------------------------------------------
    // Корректировка значения alpha при изменении знака
    // элементов  матрицы U, вычисленных  по  найденным
    // значениям alpha, beta, gamma, delta
    // -----------------------------------
    double v=gamma/2.0;
    if (
        abs(   exp(i*(alpha-beta/2.0-delta/2.0))*cos(v)-a)>EPS
        && abs(-i*exp(i*(alpha-beta/2.0+delta/2.0))*sin(v)-b)>EPS
        && abs(-i*exp(i*(alpha+beta/2.0-delta/2.0))*sin(v)-c)>EPS
        && abs(   exp(i*(alpha+beta/2.0+delta/2.0))*cos(v)-d)>EPS
        )
        alpha=M_PI+alpha;
    else;
    // -----------------------------------------------
    /*
     cout << "alpha=" << floor(alpha*180/M_PI) << endl;
     cout << "beta =" << floor(beta *180/M_PI) << endl;
     cout << "gamma=" << floor(gamma*180/M_PI) << endl;
     cout << "delta=" << floor(delta*180/M_PI) << endl << endl;
    */

    // clang-format on
}
