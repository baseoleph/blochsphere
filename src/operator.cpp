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
        operator2d op = getRX(i / DURATION * gamma);
        trace.append(actByOperator(op, q));
    }

    operator2d op = getRX(gamma);
    trace.append(actByOperator(op, q));

    qDebug() << "ab" << trace.last().a().real() << trace.last().a().imag()
             << trace.last().b().real() << trace.last().b().imag();
    std::reverse(trace.begin(), trace.end());
    return trace;
}

QVector<Qubit> Operator::rYRotate(Qubit q, double gamma) {
    QVector<Qubit> trace = {q};
    for (size_t i = 1; i < DURATION; ++i) {
        operator2d op = getRY(i / DURATION * gamma);
        trace.append(actByOperator(op, q));
    }

    operator2d op = getRY(gamma);
    trace.append(actByOperator(op, q));

    qDebug() << "ab" << trace.last().a().real() << trace.last().a().imag()
             << trace.last().b().real() << trace.last().b().imag();
    std::reverse(trace.begin(), trace.end());
    return trace;
}

QVector<Qubit> Operator::rZRotate(Qubit q, double gamma) {
    QVector<Qubit> trace = {q};
    for (size_t i = 1; i < DURATION; ++i) {
        operator2d op = getRZ(i / DURATION * gamma);
        trace.append(actByOperator(op, q));
    }

    operator2d op = getRZ(gamma);
    trace.append(actByOperator(op, q));

    qDebug() << "ab" << trace.last().a().real() << trace.last().a().imag()
             << trace.last().b().real() << trace.last().b().imag();
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
