#include "operator.h"

Operator::Operator() {
}

Qubit Operator::actByOperator(operator2d op, Qubit q) {
    return Qubit(q.a() * op[0][0] + q.b() * op[0][1], q.a() * op[1][0] + q.b() * op[1][1]);
}

// TODO pointer to function
QVector<Qubit> Operator::rXRotate(Qubit q, double gamma) {
    QVector<Qubit> trace = {q};
    for (size_t i = 1; i < DURATION; ++i) {
        trace.append(rotateX(q, i / DURATION * gamma));
    }
    trace.append(rotateX(q, gamma));
    std::reverse(trace.begin(), trace.end());
    return trace;
}

QVector<Qubit> Operator::rYRotate(Qubit q, double gamma) {
    QVector<Qubit> trace = {q};
    for (size_t i = 1; i < DURATION; ++i) {
        trace.append(rotateY(q, i / DURATION * gamma));
    }
    trace.append(rotateY(q, gamma));
    std::reverse(trace.begin(), trace.end());
    return trace;
}

QVector<Qubit> Operator::rZRotate(Qubit q, double gamma) {
    QVector<Qubit> trace = {q};
    for (size_t i = 1; i < DURATION; ++i) {
        trace.append(rotateZ(q, i / DURATION * gamma));
    }
    trace.append(rotateZ(q, gamma));
    std::reverse(trace.begin(), trace.end());
    return trace;
}

QVector<Qubit> Operator::zxRotate(Qubit q, double a, double b, double g, double d) {
    QVector<Qubit> trace = {q};
    complex        ea = exp(-complex(0, 1) * a);
    Qubit *        p = nullptr;
    operator2d     op;
    trace.append(Qubit(ea * q.a(), ea * q.b()));

    //    if (b != 0) {
    //        p = &trace.last();

    //        for (size_t i = 1; i < DURATION; ++i) {
    //            operator2d op = getRZ(i / DURATION * b);
    //            trace.append(actByOperator(op, *p));
    //        }
    //        op = getRZ(b);
    //        trace.append(actByOperator(op, *p));
    //    }

    if (g != 0) {

        p = &trace.last();
        for (size_t i = 1; i < DURATION; ++i) {
            operator2d op = getRX(i / DURATION * g);
            trace.append(actByOperator(op, *p));
        }

        op = getRX(b);
        trace.append(actByOperator(op, *p));
    }

    //    if (d != 0) {

    //        p = &trace.last();
    //        for (size_t i = 1; i < DURATION; ++i) {
    //            operator2d op = getRZ(i / DURATION * d);
    //            trace.append(actByOperator(op, *p));
    //        }
    //        op = getRZ(b);
    //        trace.append(actByOperator(op, *p));
    //    }

    std::reverse(trace.begin(), trace.end());
    return trace;
}

operator2d Operator::getRX(double gamma) {
    operator2d op = {{qCos(gamma / 2.), -complex(0, 1) * qSin(gamma / 2.)},
                     {-complex(0, 1) * qSin(gamma / 2.), qCos(gamma / 2.)}};

    return op;
}

operator2d Operator::getRY(double gamma) {
    operator2d op = {{qCos(gamma / 2.), -qSin(gamma / 2.)}, {qSin(gamma / 2.), qCos(gamma / 2.)}};

    return op;
}

operator2d Operator::getRZ(double gamma) {
    operator2d op = {{exp(-complex(0, 1) * gamma / 2.), 0}, {0, exp(complex(0, 1) * gamma / 2.)}};

    return op;
}

void Operator::printOperator(operator2d op) {
    qDebug() << "(" << op[0][0].real() << op[0][0].imag() << ") (" << op[0][1].real()
             << op[0][1].imag() << ")";
    qDebug() << "(" << op[1][0].real() << op[1][0].imag() << ") (" << op[1][1].real()
             << op[1][1].imag() << ")";
}

Qubit Operator::rotateX(Qubit q, double gamma) {
    q.changeQubit(q.x(), q.y() * cos(gamma) - q.z() * sin(gamma),
                  q.y() * sin(gamma) + q.z() * cos(gamma));
    return q;
}

Qubit Operator::rotateY(Qubit q, double gamma) {
    q.changeQubit(q.x() * cos(gamma) + q.z() * sin(gamma), q.y(),
                  -q.x() * sin(gamma) + q.z() * cos(gamma));
    return q;
}

Qubit Operator::rotateZ(Qubit q, double gamma) {
    q.changeQubit(q.x() * cos(gamma) - q.y() * sin(gamma), q.x() * sin(gamma) + q.y() * cos(gamma),
                  q.z());
    return q;
}
