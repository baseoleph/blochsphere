// A Bloch sphere emulator program.
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

#include "Operator.h"

Operator::Operator() { toId(); }

QVector<Spike> Operator::rotate(Spike s, QVector3D v, double gamma) {
    QVector<Spike> trace;
    trace.append(s);
    for (uint i = 1; i < Utility::getDuration(); ++i) {
        QQuaternion q = QQuaternion::fromAxisAndAngle(
            v, static_cast<float>(i / Utility::getDuration() * gamma * 180 / M_PI));
        trace.append(Vector::actOperator(q, s));
    }
    QQuaternion q = QQuaternion::fromAxisAndAngle(v, static_cast<float>(gamma * 180 / M_PI));
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

QVector<Spike> Operator::applyZxDecomposition(Spike s, UnitaryMatrix2x2 op) {
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

QVector<Spike> Operator::applyZyDecomposition(Spike s, UnitaryMatrix2x2 op) {
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

QVector<Spike> Operator::applyXyDecomposition(Spike s, UnitaryMatrix2x2 op) {
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

QVector<Spike> Operator::applyZyxDecomposition(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    decomposition  dec = zyxDecomposition(op);

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
        QVector<Spike> vct = rZRotate(spike.last(), dec.beta);
        for (auto &e : vct) {
            spike.append(e);
        }
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyOperator(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    if (Operator::getOperatorName(op) == "Id") {
        return spike;
    }

    decomposition dec = zyDecomposition(op);
    QVector3D     zVector = QVector3D(0, 0, 1);
    QVector3D     xVector = QVector3D(1, 0, 0);
    QQuaternion   qz1 = QQuaternion::fromAxisAndAngle(zVector, static_cast<float>(dec.beta));
    QQuaternion   qx = QQuaternion::fromAxisAndAngle(xVector, static_cast<float>(dec.gamma));
    QQuaternion   qz2 = QQuaternion::fromAxisAndAngle(zVector, static_cast<float>(dec.delta));
    QQuaternion   q = qz1 * qx * qz2;

    QVector3D v = q.vector().normalized();
    double    g = qFuzzyIsNull(v.length()) ? 0 : 2 * qAcos(q.scalar()) * 180 / M_PI;
    if (qFuzzyIsNull(g)) {
        g = g > 0 ? 180 : -180;
    }

    if (not qFuzzyIsNull(g)) {
        for (uint i = 1; i < Utility::getDuration(); ++i) {
            QQuaternion qq = QQuaternion::fromAxisAndAngle(
                v, static_cast<float>(i / Utility::getDuration() * g));
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
    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();
    double EPS = EPSILON;

    // Author  : ?????????????? ???????????? ????????????????????????
    // clang-format off

    // ****************************
    // ???????????????????? Z-Y-????????????????????...
    // ----------------------------
    // ?????????????????????????? ???????????????? alpha
    // ----------------------------
    if (abs(d)>EPS)
      if (abs(complex(1)+a/conj(d))<EPS)
        alpha=M_PI/2.0;
      else alpha=arg(a/conj(d))/2.0;
         else if (abs(c)>EPS)
           if (abs(complex(1)-b/conj(c))<EPS)
             alpha=M_PI/2.0;
           else alpha=arg(-b/conj(c))/2.0;
    // -----------------------------------
    complex A, B;
    double a1, a2, b1, b2;
    A = (exp(-i*alpha)*(a+b) + exp(i*alpha)*(conj(c)+conj(d)))/2.0;
    B = (exp(-i*alpha)*(a+b) - exp(i*alpha)*(conj(c)+conj(d)))/2.0;

    a1 = real(A); a2 = imag(A); b1 = real(B); b2 = imag(B);

    double a1n=a1, a2n=a2, b1n=b1, b2n=b2;
    a1=a1n; a2=b1n; b1=b2n; b2=a2n;       // ?????? Z-X-????????????????????
    // ------------------------------
    if (fabs(a1)<EPS && fabs(b2)<EPS)
      if (fabs(b1)>EPS)                     // a1=0, a2=?, b1!=0, b2=0
      {
        delta=0;                            // delta - ?????????? ???? R
        beta=delta + 2.0*atan2(-a2,-b1);
        gamma=M_PI;
      }
      else if (fabs(a2)>EPS)                // a1=0, a2!=0, b1=?, b2=0
           {
             delta= M_PI;                   // delta - ?????????? ???? R
             beta = delta-M_PI+2.0*atan2(-b1,a2);
             gamma= M_PI;
           }
           else;
    // ------------------------------
    if (fabs(a2)<EPS && fabs(b1)<EPS)
      if (fabs(a1)>EPS)                     // a1!=0, a2=0, b1=0, b2=?
      {
        delta=0;                            // delta - ?????????? ???? R
        beta=-delta + 2.0*atan2(-b2,a1);
        gamma=0;
      }
      else if (fabs(b2)>EPS)                // a1=?, a2=0, b1=0, b2!=0
           {
             delta=0;                       // delta - ?????????? ???? R
             beta =-delta-M_PI+2.0*atan2(a1,b2);
             gamma=0;
           }
           else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)>EPS)       // a1=0, a2!=0, b1=0, b2!=0
    {
      beta=M_PI;
      gamma=2.0*atan2(-a2,-b2);
      delta=0;
    }
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)<EPS)       // a1!=0, a2!=0, b1=0, b2=0
    {
      beta=M_PI/2.0;
      gamma=2.0*atan2(-a2,a1);
      delta=-M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)<EPS)       // a1!=0, a2=0, b1!=0, b2=0
    {
      beta=0;
      gamma=2.0*atan2(-b1,a1);
      delta=0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1!=0, a2=0, b1!=0, b2!=0
    {
      beta=atan(-b2/a1);
      gamma=2.0*atan2(-b1,-b2/sin(beta));
      delta=beta;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1!=0, a2!=0, b1!=0, b2!=0
    {
      beta =atan(-b2/a1)+atan(a2/b1);
      delta=atan(-b2/a1)-atan(a2/b1);
      gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                      a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)<EPS)       // a1!=0, a2!=0, b1!=0, b2=0
    {
      beta =atan(-b2/a1)+atan(a2/b1);
      delta=atan(-b2/a1)-atan(a2/b1);
      gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                      a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1=0, a2=0, b1!=0, b2!=0
    {
      beta=M_PI/2.0;
      gamma=2.0*atan2(-b1,-b2);
      delta=M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1=0, a2!=0, b1!=0, b2!=0
    {
      beta  = M_PI/2.0 + atan(a2/b1);
      delta = M_PI-beta;
      gamma = 2.0*atan2(-b1/cos(beta/2.0-delta/2.0),-b2);
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)>EPS)       // a1!=0, a2!=0, b1=0, b2!=0
    {
      beta  = M_PI/2.0 + atan(-b2/a1);
      delta = -M_PI + beta;
      gamma = 2.0*atan2(-a2, -b2/sin(beta/2.0+delta/2.0));
    }
    else;

    // clang-format on

    decomposition dec;
    dec.alpha = alpha;
    dec.beta = beta;
    dec.delta = delta;
    dec.gamma = gamma;
    return dec;
}

decomposition Operator::zxDecomposition() { return zxDecomposition(_op); }

decomposition Operator::zyDecomposition(UnitaryMatrix2x2 op) {
    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();
    double EPS = EPSILON;

    // Author  : ?????????????? ???????????? ????????????????????????
    // clang-format off

    // ****************************
    // ???????????????????? Z-Y-????????????????????...
    // ----------------------------
    // ?????????????????????????? ???????????????? alpha
    // ----------------------------
    if (abs(d)>EPS)
    if (abs(complex(1)+a/conj(d))<EPS)
    alpha=M_PI/2.0;
    else alpha=arg(a/conj(d))/2.0;
     else if (abs(c)>EPS)
       if (abs(complex(1)-b/conj(c))<EPS)
         alpha=M_PI/2.0;
       else alpha=arg(-b/conj(c))/2.0;
    // -----------------------------------
    complex A, B;
    double a1, a2, b1, b2;
    A = (exp(-i*alpha)*(a+b) + exp(i*alpha)*(conj(c)+conj(d)))/2.0;
    B = (exp(-i*alpha)*(a+b) - exp(i*alpha)*(conj(c)+conj(d)))/2.0;

    a1 = real(A); a2 = imag(A); b1 = real(B); b2 = imag(B);

    double a1n=a1, a2n=a2, b1n=b1, b2n=b2;
    a1=a1n; a2=-b2n; b1=b1n; b2=a2n;       // ?????? Z-Y-????????????????????
    // ------------------------------
    if (fabs(a1)<EPS && fabs(b2)<EPS)
    if (fabs(b1)>EPS)                     // a1=0, a2=?, b1!=0, b2=0
    {
    delta=0;                            // delta - ?????????? ???? R
    beta=delta + 2.0*atan2(-a2,-b1);
    gamma=M_PI;
    }
    else if (fabs(a2)>EPS)                // a1=0, a2!=0, b1=?, b2=0
       {
         delta= M_PI;                   // delta - ?????????? ???? R
         beta = delta-M_PI+2.0*atan2(-b1,a2);
         gamma= M_PI;
       }
       else;
    // ------------------------------
    if (fabs(a2)<EPS && fabs(b1)<EPS)
    if (fabs(a1)>EPS)                     // a1!=0, a2=0, b1=0, b2=?
    {
    delta=0;                            // delta - ?????????? ???? R
    beta=-delta + 2.0*atan2(-b2,a1);
    gamma=0;
    }
    else if (fabs(b2)>EPS)                // a1=?, a2=0, b1=0, b2!=0
       {
         delta=0;                       // delta - ?????????? ???? R
         beta =-delta-M_PI+2.0*atan2(a1,b2);
         gamma=0;
       }
       else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)<EPS
                 && fabs(b2)>EPS)       // a1=0, a2!=0, b1=0, b2!=0
    {
    beta=M_PI;
    gamma=2.0*atan2(-a2,-b2);
    delta=0;
    }
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                 && fabs(b2)<EPS)       // a1!=0, a2!=0, b1=0, b2=0
    {
    beta=M_PI/2.0;
    gamma=2.0*atan2(-a2,a1);
    delta=-M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                 && fabs(b2)<EPS)       // a1!=0, a2=0, b1!=0, b2=0
    {
    beta=0;
    gamma=2.0*atan2(-b1,a1);
    delta=0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                 && fabs(b2)>EPS)       // a1!=0, a2=0, b1!=0, b2!=0
    {
    beta=atan(-b2/a1);
    gamma=2.0*atan2(-b1,-b2/sin(beta));
    delta=beta;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                 && fabs(b2)>EPS)       // a1!=0, a2!=0, b1!=0, b2!=0
    {
    beta =atan(-b2/a1)+atan(a2/b1);
    delta=atan(-b2/a1)-atan(a2/b1);
    gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                  a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                 && fabs(b2)<EPS)       // a1!=0, a2!=0, b1!=0, b2=0
    {
    beta =atan(-b2/a1)+atan(a2/b1);
    delta=atan(-b2/a1)-atan(a2/b1);
    gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                  a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)<EPS && fabs(b1)>EPS
                 && fabs(b2)>EPS)       // a1=0, a2=0, b1!=0, b2!=0
    {
    beta=M_PI/2.0;
    gamma=2.0*atan2(-b1,-b2);
    delta=M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)>EPS
                 && fabs(b2)>EPS)       // a1=0, a2!=0, b1!=0, b2!=0
    {
    beta  = M_PI/2.0 + atan(a2/b1);
    delta = M_PI-beta;
    gamma = 2.0*atan2(-b1/cos(beta/2.0-delta/2.0),-b2);
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                 && fabs(b2)>EPS)       // a1!=0, a2!=0, b1=0, b2!=0
    {
    beta  = M_PI/2.0 + atan(-b2/a1);
    delta = -M_PI + beta;
    gamma = 2.0*atan2(-a2, -b2/sin(beta/2.0+delta/2.0));
    }
    else;

    // clang-format on

    decomposition dec;
    dec.alpha = alpha;
    dec.beta = beta;
    dec.delta = delta;
    dec.gamma = gamma;
    return dec;
}

decomposition Operator::zyDecomposition() { return zyDecomposition(_op); }

decomposition Operator::xyDecomposition(UnitaryMatrix2x2 op) {
    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();
    double EPS = EPSILON;

    // Author  : ?????????????? ???????????? ????????????????????????
    // clang-format off

    // *************************
    // ???????????????????? X-Y-????????????????????
    // ----------------------------
    // ?????????????????????????? ???????????????? alpha
    // ----------------------------
    if (abs(d)>EPS)
      if (abs(complex(1)+a/conj(d))<EPS)
        alpha=M_PI/2.0;
      else alpha=arg(a/conj(d))/2.0;
    else if (abs(c)>EPS)
      if (abs(complex(1)-b/conj(c))<EPS)
        alpha=M_PI/2.0;
      else alpha=arg(-b/conj(c))/2.0;
    // ------------------------------
    complex A, B;
    double a1, a2, b1, b2;
    A = (exp(-i*alpha)*(a+b) + exp(i*alpha)*(conj(c)+conj(d)))/2.0;
    B = (exp(-i*alpha)*(a+b) - exp(i*alpha)*(conj(c)+conj(d)))/2.0;

    a1 = real(A); a2 = imag(A); b1 = real(B); b2 = imag(B);
    // ------------------------------
    if (fabs(a1)<EPS && fabs(b2)<EPS)
      if (fabs(b1)>EPS)                     // a1=0, a2=?, b1!=0, b2=0
      {
        delta=0;                            // delta - ?????????? ???? R
        beta=delta + 2.0*atan2(-a2,-b1);
        gamma=M_PI;
      }
      else if (fabs(a2)>EPS)                // a1=0, a2!=0, b1=?, b2=0
           {
             delta= M_PI;                   // delta - ?????????? ???? R
             beta = delta-M_PI+2.0*atan2(-b1,a2);
             gamma= M_PI;
           }
           else;
    // ------------------------------
    if (fabs(a2)<EPS && fabs(b1)<EPS)
      if (fabs(a1)>EPS)                     // a1!=0, a2=0, b1=0, b2=?
      {
        delta=0;                            // delta - ?????????? ???? R
        beta=-delta + 2.0*atan2(-b2,a1);
        gamma=0;
      }
      else if (fabs(b2)>EPS)                // a1=?, a2=0, b1=0, b2!=0
           {
             delta=0;                       // delta - ?????????? ???? R
             beta =-delta-M_PI+2.0*atan2(a1,b2);
             gamma=0;
           }
           else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)>EPS)       // a1=0, a2!=0, b1=0, b2!=0
    {
      beta=M_PI;
      gamma=2.0*atan2(-a2,-b2);
      delta=0;
    }
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)<EPS)       // a1!=0, a2!=0, b1=0, b2=0
    {
      beta=M_PI/2.0;
      gamma=2.0*atan2(-a2,a1);
      delta=-M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)<EPS)       // a1!=0, a2=0, b1!=0, b2=0
    {
      beta=0;
      gamma=2.0*atan2(-b1,a1);
      delta=0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1!=0, a2=0, b1!=0, b2!=0
    {
      beta=atan(-b2/a1);
      gamma=2.0*atan2(-b1,-b2/sin(beta));
      delta=beta;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1!=0, a2!=0, b1!=0, b2!=0
    {
      beta =atan(-b2/a1)+atan(a2/b1);
      delta=atan(-b2/a1)-atan(a2/b1);
      gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                      a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)<EPS)       // a1!=0, a2!=0, b1!=0, b2=0
    {
      beta =atan(-b2/a1)+atan(a2/b1);
      delta=atan(-b2/a1)-atan(a2/b1);
      gamma=2.0*atan2(-a2/sin(beta/2.0-delta/2.0),
                      a1/cos(beta/2.0+delta/2.0));
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)<EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1=0, a2=0, b1!=0, b2!=0
    {
      beta=M_PI/2.0;
      gamma=2.0*atan2(-b1,-b2);
      delta=M_PI/2.0;
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)<EPS && fabs(a2)>EPS && fabs(b1)>EPS
                     && fabs(b2)>EPS)       // a1=0, a2!=0, b1!=0, b2!=0
    {
      beta  = M_PI/2.0 + atan(a2/b1);
      delta = M_PI-beta;
      gamma = 2.0*atan2(-b1/cos(beta/2.0-delta/2.0),-b2);
    }
    else;
    // ---------------------------------------------
    if (fabs(a1)>EPS && fabs(a2)>EPS && fabs(b1)<EPS
                     && fabs(b2)>EPS)       // a1!=0, a2!=0, b1=0, b2!=0
    {
      beta  = M_PI/2.0 + atan(-b2/a1);
      delta = -M_PI + beta;
      gamma = 2.0*atan2(-a2, -b2/sin(beta/2.0+delta/2.0));
    }
    else;

    // clang-format on

    decomposition dec;
    dec.alpha = alpha;
    dec.beta = beta;
    dec.delta = delta;
    dec.gamma = gamma;
    return dec;
}

decomposition Operator::xyDecomposition() { return xyDecomposition(_op); }

decomposition Operator::zyxDecomposition(UnitaryMatrix2x2 op) {
    complex i = complex(0, 1);
    double  alpha = 0;
    double  beta = 0;
    double  delta = 0;
    double  gamma = 0;
    complex a = op.a();
    complex b = op.b();
    complex c = op.c();
    complex d = op.d();
    double EPS = EPSILON;

    //  Author  : ?????????????? ???????????? ????????????????????????
    // clang-format off

    // ***************************
    // ???????????????????? Z-Y-X-????????????????????
    // ---------------------------
    // ???????????????????? ???????? alpha
    // ---------------------
    if (abs(d)>EPS)
    if (abs(complex(1)+a/conj(d))<EPS)
    alpha=M_PI/2.0;
    else alpha=arg(a/conj(d))/2.0;
    else if (abs(c)>EPS)
     if (abs(complex(1)-b/conj(c))<EPS)
            alpha=M_PI/2.0;
     else alpha=arg(-b/conj(c))/2.0;
    // -----------------------------------
    complex A,B;
    A=(exp(-i*alpha)*(a+b) + exp(i*alpha)*(conj(c)+conj(d)))/2.0;
    B=(exp(-i*alpha)*(a+b) - exp(i*alpha)*(conj(c)+conj(d)))/2.0;
    // ----------------------------------------------
    double a1, a2, b1, b2;
    a1=real(A); a2=imag(A); b1=real(B); b2=imag(B);
    // -----------------------------------------------------
    // ?????????????????????? (????????????????!) ?????????????????????????????? ????????????????????...
    // -----------------------------------------------------
    double u1, u2, u3, u4;
    u1=a1+b1; u2=a1-b1; u3=a2+b2; u4=a2-b2;
    // ---------------------------------------------
    if (fabs(u1)<EPS && fabs(u3)<EPS && fabs(u2)>EPS)
    {
    gamma=M_PI/2.0;
    delta=0;       // delta - ?????????? ???? R
    if (-u4>0)
    beta=delta + 2.0*acos(1.0/sqrt(2.0)*u2);     // (!)
    else beta=delta - 2.0*acos(1.0/sqrt(2.0)*u2);  // (!)
    }
    else if (fabs(u1)<EPS && fabs(u3)<EPS && fabs(u4)>EPS)
    {
     gamma=M_PI/2.0;
     delta=0;                // delta - ?????????? ???? R
     beta=delta - M_PI + 2.0*atan2(1.0/sqrt(2.0)*u2,
                                   1.0/sqrt(2.0)*u4);
    }
    else;
    // ----------------------------------------------
    if (fabs(u4)<EPS && fabs(u2)<EPS && fabs(u1)>EPS)
    {
    gamma=-M_PI/2;                // ?????? gamma=3*M_PI/2;
    delta=0;                      // delta - ?????????? ???? R
    beta=-delta + 2.0*atan2(-1.0/sqrt(2.0)*u3,
                         1.0/sqrt(2.0)*u1);
    }
    else if (fabs(u4)<EPS && fabs(u2)<EPS && fabs(u3)>EPS)
    {
     gamma=-M_PI/2.0;
     delta=0;           // delta - ?????????? ???? R
     beta=-delta-M_PI+2.0*atan2(1.0/sqrt(2.0)*u1,
                                1.0/sqrt(2.0)*u3);
    }
    else;
    // -----------------------------
    if (fabs(u1)<EPS && fabs(u4)>EPS
               && fabs(u2)<EPS && fabs(u3)>EPS)
    {
    beta=M_PI;
    delta=0;
    gamma = 2.0*atan2((u3-u4)/2.0,-(u3+u4)/2.0);
    }
    else;
    // -----------------------------
    if (fabs(u1)>EPS && fabs(u4)>EPS
               && fabs(u2)<EPS && fabs(u3)<EPS)
    {
    beta = M_PI/2.0;
    delta=-M_PI/2.0;
    gamma = 2.0*atan2(-(u1+u4)/2.0, (u1-u4)/2.0);
    }
    else;
    // ---------------------------------------------
    if (fabs(u1)>EPS && fabs(u4)<EPS && fabs(u2)>EPS
               && fabs(u3)<EPS)
    {
    beta=0;
    delta=0;
    // ---------------------------------------------
    // ?????????????? ?????????????? ???????????????????????????????????? ??????????????????:
    //  sin(gamma/2)=(u2-u1)/2;
    //  cos(gamma/2)=(u2+u1)/2;
    // -----------------------------------------
    gamma = 2.0*atan2((u2-u1)/2.0, (u2+u1)/2.0);
    }
    else;
    // ----------------------------------------------
    if (fabs(u1)>EPS && fabs(u2)>EPS && fabs(u3)>EPS)
                            // && fabs(u4)>EPS)
    {
    beta =-atan(u3/u1)-atan(u4/u2);
    delta=-atan(u3/u1)+atan(u4/u2);
    gamma = M_PI/2.0
          + 2.0 * atan2(sqrt(2.0)/2.0 * u3/sin(beta/2+delta/2),
                        sqrt(2.0)/2.0 * u2/cos(beta/2-delta/2));
    }
    else;
    // -----------------------------
    if (fabs(u1)<EPS && fabs(u4)<EPS
               && fabs(u2)>EPS && fabs(u3)>EPS)
    {
    beta = M_PI/2.0;
    delta= M_PI/2.0;
    gamma= 2.0*atan2((u2+u3)/2.0, (u2-u3)/2.0);
    }
    else;
    // -----------------------------
    if (fabs(u1)<EPS && fabs(u4)>EPS
               && fabs(u2)>EPS && fabs(u3)>EPS)
    {
    beta = M_PI/2.0 - atan(u4/u2);
    delta= M_PI/2.0 + atan(u4/u2);
    gamma=2.0*atan2(( u3 + u2/cos(beta/2.0-delta/2.0))/2.0,
                (-u3 + u2/cos(beta/2.0-delta/2.0))/2.0);
    }
    else;
    // -----------------------------
    if (fabs(u1)>EPS && fabs(u4)>EPS
               && fabs(u2)<EPS && fabs(u3)>EPS)
    {
    beta =M_PI/2.0 + atan(-u3/u1);
    delta=-M_PI+beta;
    gamma=2.0*atan2((-u1/cos(beta/2.0+delta/2.0)-u4)/2.0,
                ( u1/cos(beta/2.0+delta/2.0)-u4)/2.0);
    }
    else;
    // -----------------------------
    if (fabs(u1)>EPS && fabs(u4)>EPS
               && fabs(u2)>EPS && fabs(u3)<EPS)
    {
    beta =-atan(u4/u2);
    delta=-beta;
    gamma=2.0*atan2((-u4/sin(beta/2.0-delta/2.0)-u1)/2.0,
                (-u4/sin(beta/2.0-delta/2.0)+u1)/2.0);
    }
    else;

    // clang-format on

    decomposition dec;
    dec.alpha = alpha;
    dec.beta = beta;
    dec.delta = delta;
    dec.gamma = gamma;
    return dec;
}

decomposition Operator::zyxDecomposition() { return zyxDecomposition(_op); }

vectorangle Operator::vectorAngleDec(UnitaryMatrix2x2 op) {
    decomposition zyDec = Operator::zyDecomposition(op);

    zyDec.gamma = zyDec.gamma;
    zyDec.beta = zyDec.beta;
    zyDec.delta = zyDec.delta;

    vectorangle va;

    va.angle = 2.0 * acos(cos(zyDec.gamma / 2.0) * cos(zyDec.beta / 2.0 + zyDec.delta / 2.0));
    double sint = sin(va.angle / 2.0);
    if (std::abs(sint) > EPSILON) {
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
    complex          i{0, 1};

    if (seed == 0) {
        seed = time(nullptr) * rand();
    }
    srand(seed);
    double a1 = Utility::random(0., 1.);
    double a2 = Utility::random(0., 1. - a1 * a1);
    double b1 = Utility::random(0., 1. - a1 * a1 - a2 * a2);
    double b2 = sqrt(1 - a1 * a1 - a2 * a2 - b1 * b1);

    double phi = Utility::random(0., 360.);

    matrix.a = exp(i * phi) * complex(a1, a2);
    matrix.b = exp(i * phi) * complex(b1, b2);
    matrix.c = -exp(i * phi) * conj(matrix.b);
    matrix.d = exp(i * phi) * conj(matrix.a);

    op.updateMatrix(matrix);
    return op;
}

void Operator::setOperator(UnitaryMatrix2x2 op, QString opName) {
    _op = op;
    _opName = opName;
}

QVector<Spike> Operator::applyOperator(Spike s) { return applyOperator(s, _op); }

QVector<Spike> Operator::applyVectorRotation(Spike s, UnitaryMatrix2x2 op) {
    QVector<Spike> spike = {s};
    if (Operator::getOperatorName(op) == "Id") {
        return spike;
    }

    vectorangle va = vectorAngleDec(op);

    QVector<Spike> vct = rotate(s, QVector3D(va.x, va.y, va.z), va.angle);
    for (auto &e : vct) {
        spike.append(e);
    }

    std::reverse(spike.begin(), spike.end());
    return spike;
}

QVector<Spike> Operator::applyVectorRotation(Spike s) { return applyVectorRotation(s, _op); }

void Operator::toId() { setOperator(UnitaryMatrix2x2::getId(), "Id"); }
void Operator::toX() { setOperator(UnitaryMatrix2x2::getX(), "X"); }
void Operator::toY() { setOperator(UnitaryMatrix2x2::getY(), "Y"); }
void Operator::toZ() { setOperator(UnitaryMatrix2x2::getZ(), "Z"); }
void Operator::toH() { setOperator(UnitaryMatrix2x2::getH(), "H"); }
void Operator::toS() { setOperator(UnitaryMatrix2x2::getS(), "S"); }
void Operator::toT() { setOperator(UnitaryMatrix2x2::getT(), "T"); }
void Operator::toPhi(double gamma) {
    setOperator(UnitaryMatrix2x2::getPhi(gamma),
                "Phi(" + QString::number(gamma * 180 / M_PI) + ")");
}
void Operator::toXrotate(double the) {
    setOperator(UnitaryMatrix2x2::getXrotate(the), "Rx(" + QString::number(the * 180 / M_PI) + ")");
}
void Operator::toYrotate(double the) {
    setOperator(UnitaryMatrix2x2::getYrotate(the), "Ry(" + QString::number(the * 180 / M_PI) + ")");
}
void Operator::toZrotate(double the) {
    setOperator(UnitaryMatrix2x2::getZrotate(the), "Rz(" + QString::number(the * 180 / M_PI) + ")");
}

QVector<Spike> Operator::applyZxDecomposition(Spike s) { return applyZxDecomposition(s, _op); }
QVector<Spike> Operator::applyZyDecomposition(Spike s) { return applyZyDecomposition(s, _op); }
QVector<Spike> Operator::applyXyDecomposition(Spike s) { return applyXyDecomposition(s, _op); }
QVector<Spike> Operator::applyZyxDecomposition(Spike s) { return applyZyxDecomposition(s, _op); }

bool Operator::setOperatorByZxDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByZxDec(dec))) {
        return false;
    }
    setOperator(matrixOp, getOperatorName(matrixOp));
    return true;
}

bool Operator::setOperatorByZyDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByZyDec(dec))) {
        return false;
    }
    setOperator(matrixOp, getOperatorName(matrixOp));
    return true;
}

bool Operator::setOperatorByXyDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByXyDec(dec))) {
        return false;
    }
    setOperator(matrixOp, getOperatorName(matrixOp));
    return true;
}

bool Operator::setOperatorByZyxDecomposition(decomposition dec) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByZyxDec(dec))) {
        return false;
    }
    setOperator(matrixOp, getOperatorName(matrixOp));
    return true;
}

bool Operator::setOperatorByVectorAngle(vectorangle va) {
    UnitaryMatrix2x2 matrixOp;
    if (not matrixOp.updateMatrix(getMatrixByVecAng(va))) {
        return false;
    }
    setOperator(matrixOp, getOperatorName(matrixOp));
    return true;
}

matrix2x2 Operator::getMatrixByZxDec(decomposition dec) {
    double    alpha = dec.alpha;
    double    beta = dec.beta;
    double    delta = dec.delta;
    double    v = dec.gamma / 2;
    matrix2x2 matrix;
    complex   i{0, 1};
    matrix.a = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    matrix.b = -i * exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    matrix.c = -i * exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    matrix.d = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    return matrix;
}

matrix2x2 Operator::getMatrixByZyDec(decomposition dec) {
    double    alpha = dec.alpha;
    double    beta = dec.beta;
    double    delta = dec.delta;
    double    v = dec.gamma / 2;
    matrix2x2 matrix;
    complex   i{0, 1};
    matrix.a = exp(i * (alpha - beta / 2.0 - delta / 2.0)) * cos(v);
    matrix.b = -exp(i * (alpha - beta / 2.0 + delta / 2.0)) * sin(v);
    matrix.c = exp(i * (alpha + beta / 2.0 - delta / 2.0)) * sin(v);
    matrix.d = exp(i * (alpha + beta / 2.0 + delta / 2.0)) * cos(v);
    return matrix;
}

matrix2x2 Operator::getMatrixByXyDec(decomposition dec) {
    double    alpha = dec.alpha;
    double    beta = dec.beta;
    double    delta = dec.delta;
    double    v = dec.gamma / 2;
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

matrix2x2 Operator::getMatrixByZyxDec(decomposition dec) {
    double    alpha = dec.alpha;
    double    beta = dec.beta;
    double    delta = dec.delta;
    double    gamma = dec.gamma;
    matrix2x2 matrix;
    complex   i{0, 1};
    double    g = gamma / 2.0;
    double    d1 = delta / 2.0;

    matrix.a = exp(i * alpha - i * beta / 2.0) * (cos(g) * cos(d1) + i * sin(g) * sin(d1));
    matrix.b = exp(i * alpha - i * beta / 2.0) * (-sin(g) * cos(d1) - i * cos(g) * sin(d1));
    matrix.c = exp(i * alpha + i * beta / 2.0) * (sin(g) * cos(d1) - i * cos(g) * sin(d1));
    matrix.d = exp(i * alpha + i * beta / 2.0) * (cos(g) * cos(d1) - i * sin(g) * sin(d1));
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

QString Operator::getCurOperatorMatrixStr(UnitaryMatrix2x2 op) {
    return "<style>"
           "table, th, td {"
           "border: 1px solid black;"
           "border-collapse: collapse;"
           "padding: 5px;"
           "white-space: nowrap;"
           "}"
           "</style>"
           "<table>"
           "<tr>"
           "<td>" +
           op.aStr() +
           "</td>"
           "<td>" +
           op.bStr() +
           "</td>"
           "</tr><tr>"
           "<td>" +
           op.cStr() +
           "</td>"
           "<td>" +
           op.dStr() +
           "</td>"
           "</tr></table>";
}

QString Operator::getCurOperatorMatrixStr() { return getCurOperatorMatrixStr(_op); }

QString Operator::getOperatorName(UnitaryMatrix2x2 op) {
    QString opName = "U";
    if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getId(), op, false)) {
        opName = "Id";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getX(), op, false)) {
        opName = "X";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getY(), op, false)) {
        opName = "Y";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getZ(), op, false)) {
        opName = "Z";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getH(), op, false)) {
        opName = "H";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getS(), op, false)) {
        opName = "S";
    } else if (UnitaryMatrix2x2::compareOperators(UnitaryMatrix2x2::getT(), op, false)) {
        opName = "T";
    }
    return opName;
}
void Operator::toRandUnitaryMatrix() {
    UnitaryMatrix2x2 matrix = genRandUnitaryMatrix();
    setOperator(matrix, getOperatorName(matrix));
}
QString Operator::getOperatorName() { return _opName; }
